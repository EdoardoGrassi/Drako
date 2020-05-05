#pragma once
#ifndef DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP_
#define DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP_

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_utils.hpp"

#include <array>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    class global_allocator
    {
    public:
        explicit global_allocator(const context& ctx);

        global_allocator(const global_allocator&) = delete;
        global_allocator& operator=(const global_allocator&) = delete;

        global_allocator(global_allocator&&) = delete;
        global_allocator& operator=(global_allocator&&) = delete;


        void bind_non_sparse(vk::Buffer desc) noexcept;

        void bind_non_sparse(vk::Buffer desc, vk::MemoryPropertyFlags flags) noexcept;

    private:
        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;

        // std::array<vk::MemoryType, VK_MAX_MEMORY_TYPES> _mem_types;
        // std::array<vk::MemoryHeap, VK_MAX_MEMORY_HEAPS> _mem_heaps;
        std::array<vulkan_memory_pool, VK_MAX_MEMORY_TYPES> _mem_pools;
    };

    global_allocator::global_allocator(const context& ctx)
        : _pdevice(ctx.physical_device), _ldevice(ctx.logical_device.get())
    {
        const auto properties = _pdevice.getMemoryProperties();
        for (uint32_t i = 0; i < properties.memoryHeapCount; ++i)
        {
            const auto heap = properties.memoryHeaps[i];

            const vk::MemoryAllocateInfo alloc_info{
                vk::DeviceSize{ heap.size / 2 }, // reserve half of heap capacity
                heap.
            };
            const auto mem = _ldevice.allocateMemoryUnique(alloc_info);
        }
    }

    void global_allocator::bind_non_sparse(vk::Buffer desc) noexcept
    {
        const auto buffer_mem_req = _ldevice.getBufferMemoryRequirements(desc);
        DRAKO_ASSERT(buffer_mem_req.memoryTypeBits != 0, "Required by Vulkan standard");
        DRAKO_ASSERT(buffer_mem_req.alignment % 2 == 0, "Required by Vulkan standard");

        for (uint32_t mem_type_index = 0; mem_type_index < sizeof(decltype(buffer_mem_req.memoryTypeBits)) * 4;)
        {
            while (buffer_mem_req.memoryTypeBits & (1 << mem_type_index))
                ++mem_type_index;

            auto&      mem_pool = _mem_pools[mem_type_index];
            const auto mem_bind = mem_pool.allocate(buffer_mem_req.size, buffer_mem_req.alignment);

            if (const auto err = _ldevice.bindBufferMemory(desc, mem_bind.memory, mem_bind.offset);
                err == vk::Result::eSuccess)
            {
                return;
            }
        }
        DRAKO_LOG_FAILURE("[Vulkan] Failed to allocate suitable memory");
        std::exit(EXIT_FAILURE);
    }


    // Allocator for buffers with frequent device (GPU) usage
    class device_local_allocator
    {
    public:
        explicit device_local_allocator(const context& ctx, size_t bytes);

        device_local_allocator(const device_local_allocator&) = delete;
        device_local_allocator& operator=(const device_local_allocator&) = delete;

        device_local_allocator(device_local_allocator&&) = delete;
        device_local_allocator operator=(device_local_allocator&&) = delete;


        void allocate(vk::Buffer buffer);
        void allocate(const std::vector<vk::Buffer>& buffers);

        void deallocate(vk::Buffer buffer);
        void deallocate(const std::vector<vk::Buffer>& buffers);

    private:
        const vk::Device       _device;
        vk::UniqueDeviceMemory _memory;
        vk::UniqueBuffer       _buffer;
    };

    device_local_allocator::device_local_allocator(const context& ctx, size_t bytes) noexcept
        : _device{ ctx.logical_device.get() }
    {
        DRAKO_ASSERT(bytes > 0);

        // prototype of typical ubo use case
        const vk::BufferCreateInfo buffer_info{
            {},
            vk::DeviceSize{ bytes },
            vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::SharingMode::eConcurrent,
            static_cast<uint32_t>(std::size(queues)),
            queues.data()
        };
        if (auto [err, buffer] = _device.createBufferUnique(buffer_info);
            err == vk::Result::eSuccess)
        {
            _buffer = std::move(buffer);
        }
        else
            DRAKO_LOG_ERROR("[Vulkan] Failed buffer creation");

        const auto specs = _device.getBufferMemoryRequirements(_buffer.get());

        const vk::MemoryAllocateInfo device_alloc_info{
            specs.size,
        };
        if (auto [err, memory] = _device.allocateMemoryUnique(device_alloc_info);
            err == vk::Result::eSuccess)
            _memory = std::move(memory);
        else
            DRAKO_LOG_ERROR("[Vulkan] Failed memory allocation");
    }

    void device_local_allocator::allocate(vk::Buffer buffer)
    {
        const auto specs = _device.getBufferMemoryRequirements(buffer);

        _device.bindBufferMemory(buffer, _memory.get(), 0);
    }



    struct allocation
    {
        vk::DeviceMemory memory;
        vk::DeviceSize   offset;
    };


    // Allocator for buffers with frequent host (CPU) access
    class host_mapped_allocator
    {
    public:
        explicit host_mapped_allocator(const context& ctx, size_t bytes) noexcept;

        host_mapped_allocator(const host_mapped_allocator&) = delete;
        host_mapped_allocator& operator=(const host_mapped_allocator&) = delete;

        host_mapped_allocator(host_mapped_allocator&&) = delete;
        host_mapped_allocator& operator=(host_mapped_allocator&&) = delete;

        ~host_mapped_allocator() noexcept;

        [[nodiscard]] void* allocate(vk::Buffer b) noexcept;

        void deallocate(void* ptr) noexcept;

    private:
        const vk::Device _device;
        vk::DeviceMemory _device_memory;
        void*            _mapped_memory;
        size_t           _offset;
    };

    host_mapped_allocator::host_mapped_allocator(const context& ctx, size_t bytes) noexcept
        : _device{ ctx.logical_device.get() }
    {
        DRAKO_ASSERT(bytes > 0);

        const vk::MemoryAllocateInfo device_alloc_info{
            vk::DeviceSize{ bytes },
        };
        if (const auto [err, memory] = _device.allocateMemory(device_alloc_info);
            err == vk::Result::eSuccess)
        {
            _device_memory = memory;
        }
        else
            DRAKO_LOG_ERROR("[Vulkan] Failed memory allocation");

        if (const auto [err, ptr] = _device.mapMemory(
                _device_memory, vk::DeviceSize{ 0 }, vk::DeviceSize{ VK_WHOLE_SIZE }, {});
            err == vk::Result::eSuccess)
        {
        }
        else
            DRAKO_LOG_ERROR("[Vulkan] Failed memory mapping");
    }

    host_mapped_allocator::~host_mapped_allocator()
    {
        _device.freeMemory(_device_memory);
    }

    [[nodiscard]] void* host_mapped_allocator::allocate(vk::Buffer buffer) noexcept
    {
        DRAKO_ASSERT(buffer != VK_NULL_HANDLE);
        const auto specs = _device.getBufferMemoryRequirements(buffer);

        // TODO: change allocation algorithm, this is only for prototyping

        _offset += specs.size;
        return _mapped_memory + _offset;
    }

    void host_mapped_allocator::deallocate(void* ptr) noexcept
    {
        _device.unmapMemory()
    }




    class fixed_host_ubo
    {
    public:
        explicit fixed_host_ubo(
            const context& ctx, size_t size, host_mapped_allocator& a) noexcept
            : _device(ctx.logical_device.get()), _alloc(a), _size(size)
        {
            vk::BufferCreateInfo buffer_create_info{
                {},
                vk::DeviceSize{ size },
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::SharingMode::eConcurrent,
                static_cast<uint32_t>(std::size(queues)),
                queues.data()
            };
            if (auto [err, buffer] = _device.createBufferUnique(buffer_create_info);
                err == vk::Result::eSuccess)
            {
                _buffer = std::move(buffer);
            }
            else
            {
                DRAKO_LOG_ERROR("[Vulkan] Failed buffer creation");
            }
            _memory = _alloc.allocate(_buffer.get());

            _device.bindBufferMemory(_buffer.get(), )
        }

        ~fixed_host_ubo() noexcept
        {
            _alloc.deallocate(_memory);
        }

    private:
        const vk::Device       _device;
        host_mapped_allocator& _alloc;
        void*                  _memory;
        size_t                 _size;
        vk::UniqueBuffer       _buffer;
    };


    class fixed_device_ubo
    {
    };

} // namespace drako::gpx::vulkan

#endif // !DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP_