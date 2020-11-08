#pragma once
#ifndef DRAKO_VULKAN_STAGING_ENGINE_HPP
#define DRAKO_VULKAN_STAGING_ENGINE_HPP

#include "drako/graphics/vulkan_queue.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_utils.hpp"

#include <cassert>
#include <functional>
#include <span>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    struct memory_transfer_info
    {
        std::span<const std::byte> source;
        vk::Buffer                 destination;
        std::size_t                offset;
    };

    class staging_engine
    {
    public:
        using transfer_completed_callback = std::function<void()>;

        staging_engine(
            const context& ctx, std::size_t bytes, const transfer_queue& queue);

        staging_engine(const staging_engine&) = delete;
        staging_engine& operator=(const staging_engine&) = delete;

        ~staging_engine() noexcept;

        void submit_for_transfer(const memory_transfer_info&) noexcept;
        void submit_for_transfer(const memory_transfer_info&, transfer_completed_callback) noexcept;

        void update() noexcept;

    private:
        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;
        const transfer_queue     _queue;
        vk::UniqueBuffer         _staging_buffer;
        vk::UniqueDeviceMemory   _device_memory;
        std::span<std::byte>     _host_memory;
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

    staging_engine::staging_engine(
        const context& ctx, size_t bytes, const transfer_queue& queue)
        : _pdevice(ctx.physical_device)
        , _ldevice(ctx.logical_device.get())
    {
        assert(bytes > 0);

        _wait_for_submit_transfers.reserve(32);
        _wait_for_submit_callbacks.reserve(32);
        _wait_for_completition_callbacks.reserve(32);

        {
            const vk::BufferCreateInfo staging_buffer_info{
                vk::BufferCreateFlagBits{},
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
            _device_memory = _ldevice.allocateMemoryUnique(alloc_info);
            _ldevice.bindBufferMemory(_staging_buffer.get(), _device_memory.get(), vk::DeviceSize{ 0 });

            const auto ptr = _ldevice.mapMemory(_device_memory.get(), 0, VK_WHOLE_SIZE, {});
            _host_memory   = { static_cast<std::byte*>(ptr), bytes };
        }

        {
            const vk::CommandPoolCreateInfo command_pool_create_info{
                vk::CommandPoolCreateFlagBits::eTransient,
                _queue.family
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
            assert(std::size(buffers) == 1);
            _transfer_cmd_buffer = std::move(buffers[0]);
        }

        {
            const vk::FenceCreateInfo fence_create_info{}; // init as unsignaled
            _transfer_completed = _ldevice.createFenceUnique(fence_create_info);
        }
    }

    staging_engine::~staging_engine() noexcept
    {
        _ldevice.unmapMemory(_device_memory.get());
    }

    void staging_engine::submit_for_transfer(
        const memory_transfer_info& info) noexcept
    {
        _wait_for_submit_transfers.push_back(info);
    }

    void staging_engine::submit_for_transfer(
        const memory_transfer_info& info, transfer_completed_callback cb) noexcept
    {
        assert(cb != nullptr);

        _wait_for_submit_transfers.push_back(info);
        _wait_for_submit_callbacks.push_back(cb);
    }

    void staging_engine::update() noexcept
    {
        switch (const auto r = _ldevice.getFenceStatus(_transfer_completed.get()); r)
        {
            case vk::Result::eSuccess:
                break;
            case vk::Result::eNotReady: // queue is still busy
                return;
            default:
                log_and_exit("[Vulkan] Unexpected result " + to_string(r));
        }

        // previous batch has finished execution
        for (const auto& callback : _wait_for_completition_callbacks)
            std::invoke(callback);
        _wait_for_completition_callbacks.clear();


        _transfer_cmd_buffer.get().begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr });

        /*
        auto curr_alloc = _host_memory.data();
        for (size_t batch_size = 0, last_request_index = 0; batch_size < _host_memory.size_bytes(); ++last_request_index)
        {
            const auto& transfer = _wait_for_submit_transfers[last_request_index];
            std::memcpy(_host_memory.data(), transfer.source.data(), transfer.source.size_bytes());
            curr_alloc += transfer.source.size_bytes();

            vk::BufferCopy region{};
            region.size      = transfer.source.size_bytes();
            region.srcOffset = transfer.batch_size;
            region.dstOffset = transfer.offset;
            _transfer_cmd_buffer.get().copyBuffer(_staging_buffer.get(), transfer.destination, 1, &region);
        }*/

        _transfer_cmd_buffer.get().end();

        const auto FLUSH_NOT_REQUIRED = vk::MemoryPropertyFlagBits::eHostCoherent |
                                        vk::MemoryPropertyFlagBits::eDeviceCoherentAMD;
        if (!(_staging_memory_specs & FLUSH_NOT_REQUIRED))
        { // request explicit flush
            const vk::MappedMemoryRange range{
                _device_memory.get(),
                0,
                vk::DeviceSize{ batch_size }
            };
            DRAKO_VK_CHECK_SUCCESS(_ldevice.flushMappedMemoryRanges(1, &range));
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

#endif // !DRAKO_VULKAN_STAGING_ALLOCATOR_HPP