#pragma once
#ifndef DRAKO_VULKAN_STAGING_ALLOCATOR_HPP
#define DRAKO_VULKAN_STAGING_ALLOCATOR_HPP

#include "drako/devel/assertion.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_utils.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    struct memory_transfer_info
    {
        const std::byte* source;
        size_t           size;
        vk::Buffer       destination;
        size_t           offset;
    };

    class staging_allocator
    {
    public:
        using transfer_completed_callback = void (*)();

        staging_allocator(
            const context& ctx, size_t bytes, const std::vector<std::uint32_t>& families);

        staging_allocator(const staging_allocator&) = delete;
        staging_allocator& operator=(const staging_allocator&) = delete;

        staging_allocator(staging_allocator&&) = delete;
        staging_allocator& operator=(staging_allocator&&) = delete;

        ~staging_allocator() noexcept;

        void submit_for_transfer(const memory_transfer_info&) noexcept;
        void submit_for_transfer(const memory_transfer_info&, transfer_completed_callback) noexcept;

        void update() noexcept;

    private:
        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;
        vk::UniqueBuffer         _staging_buffer;
        vk::UniqueDeviceMemory   _staging_memory;
        size_t                   _staging_memory_size;
        std::byte*               _staging_memory_host_view;
        vk::MemoryPropertyFlags  _staging_memory_specs;

        vk::UniqueCommandPool   _transfer_cmd_pool;
        vk::UniqueCommandBuffer _transfer_cmd_buffer;
        vk::Queue               _transfer_queue;
        vk::UniqueFence         _transfer_completed;

        std::vector<memory_transfer_info>        _wait_for_submit_transfers;
        std::vector<transfer_completed_callback> _wait_for_submit_callbacks;
        std::vector<transfer_completed_callback> _wait_for_completition_callbacks;

        void _submit_batch() noexcept;
    };

    staging_allocator::staging_allocator(
        const context& ctx, size_t bytes, const std::vector<std::uint32_t>& families)
        : _pdevice(ctx.physical_device)
        , _ldevice(ctx.logical_device.get())
    {
        DRAKO_ASSERT(bytes > 0);
        DRAKO_ASSERT(!std::empty(families));

        _wait_for_submit_transfers.reserve(32);
        _wait_for_submit_callbacks.reserve(32);
        _wait_for_completition_callbacks.reserve(32);

        {
            const vk::BufferCreateInfo staging_buffer_info{
                vk::BufferCreateFlagBits::eSparseBinding,
                vk::DeviceSize{ bytes },
                vk::BufferUsageFlagBits::eTransferSrc,
                vk::SharingMode::eConcurrent,
                0, nullptr /* Not needed for exclusive ownership */
            };
            _staging_buffer = _ldevice.createBufferUnique(staging_buffer_info);
        }

        {
            const auto mem_loader_specs = vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent;
            const auto mem_buffer_specs = _ldevice.getBufferMemoryRequirements(_staging_buffer.get());
            const auto properties       = _pdevice.getMemoryProperties();

            uint32_t i = 0;
            for (; i < properties.memoryTypeCount; ++i)
            {
                if ((mem_buffer_specs.memoryTypeBits & (1 << i)) &&
                    (properties.memoryTypes[i].propertyFlags & mem_loader_specs))
                {
                    _staging_memory_specs = properties.memoryTypes[i].propertyFlags;
                    break;
                }
            }
            const vk::MemoryAllocateInfo alloc_info{ vk::DeviceSize{ bytes }, i };
            _staging_memory = _ldevice.allocateMemoryUnique(alloc_info);
            _ldevice.bindBufferMemory(_staging_buffer.get(), _staging_memory.get(), vk::DeviceSize{ 0 });

            _staging_memory_host_view = static_cast<std::byte*>(
                _ldevice.mapMemory(_staging_memory.get(), 0, VK_WHOLE_SIZE, {}));
        }

        {
            const vk::CommandPoolCreateInfo command_pool_create_info{
                vk::CommandPoolCreateFlagBits::eTransient,
                _queue_family_index
            };
            _transfer_cmd_pool = _ldevice.createCommandPoolUnique(command_pool_create_info);
        }

        {
            const vk::CommandBufferAllocateInfo command_buffer_alloc_info{
                _transfer_cmd_pool.get(),
                vk::CommandBufferLevel::ePrimary,
                1 // command buffers count
            };
            auto buffers = _ldevice.allocateCommandBuffersUnique(command_buffer_alloc_info);
            DRAKO_ASSERT(std::size(buffers) == 1);
            _transfer_cmd_buffer = std::move(buffers[0]);
        }

        {
            const vk::FenceCreateInfo fence_create_info{}; // init as unsignaled
            _transfer_completed = _ldevice.createFenceUnique(fence_create_info);
        }
    }

    staging_allocator::~staging_allocator() noexcept
    {
        _ldevice.unmapMemory(_staging_memory.get());
    }

    void staging_allocator::submit_for_transfer(
        const memory_transfer_info& info) noexcept
    {
        _wait_for_submit_transfers.push_back(info);
    }

    void staging_allocator::submit_for_transfer(
        const memory_transfer_info& info, transfer_completed_callback cb) noexcept
    {
        DRAKO_ASSERT(cb != nullptr);

        _wait_for_submit_transfers.push_back(info);
        _wait_for_submit_callbacks.push_back(cb);
    }

    void staging_allocator::update() noexcept
    {
        switch (_ldevice.getFenceStatus(_transfer_completed.get()))
        {
            case vk::Result::eSuccess:
                break;
            case vk::Result::eNotReady: // queue is still busy
                return;
            default:
                DRAKO_LOG_ERROR("");
                std::exit(EXIT_FAILURE);
        }

        // previous batch has finished execution
        for (const auto& callback : _wait_for_completition_callbacks)
            std::invoke(callback);

        _wait_for_completition_callbacks.clear();

        const vk::CommandBufferBeginInfo cmd_begin_info{
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
            nullptr
        };
        _transfer_cmd_buffer.get().begin(cmd_begin_info);

        auto   curr_alloc         = _staging_memory_host_view;
        size_t batch_size         = 0;
        size_t last_request_index = 0;
        for (; batch_size < _staging_memory_size; ++last_request_index)
        {
            const auto& transfer = _wait_for_submit_transfers[last_request_index];
            std::memcpy(_staging_memory_host_view, transfer.source, transfer.size);
            curr_alloc += transfer.size;

            vk::BufferCopy region{};
            region.size      = transfer.size;
            region.srcOffset = transfer.batch_size;
            region.dstOffset = transfer.offset;
            _transfer_cmd_buffer.get().copyBuffer(_staging_buffer.get(), transfer.destination, 1, &region);
        }

        _transfer_cmd_buffer.get().end();

        const auto FLUSH_NOT_REQUIRED = vk::MemoryPropertyFlagBits::eHostCoherent |
                                        vk::MemoryPropertyFlagBits::eDeviceCoherentAMD;
        if (!(_staging_memory_specs & FLUSH_NOT_REQUIRED))
        { // request explicit flush
            const vk::MappedMemoryRange mapped_range{
                _staging_memory.get(),
                0,
                vk::DeviceSize{ batch_size }
            };
            DRAKO_VK_CHECK_SUCCESS(_ldevice.flushMappedMemoryRanges(1, &mapped_range));
        }

        _ldevice.resetFences(1, &_transfer_completed.get());

        {
            vk::SubmitInfo job{};
            job.commandBufferCount = 1;
            job.pCommandBuffers    = &_transfer_cmd_buffer.get();
            _transfer_queue.submit(1, &job, _transfer_completed.get());
        }
    }
} // namespace drako::gpx::vulkan

} // namespace drako::gpx::vulkan

#endif // !DRAKO_VULKAN_STAGING_ALLOCATOR_HPP