#pragma once
#ifndef DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP
#define DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP

#include "drako/devel/logging.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_utils.hpp"

#include <array>
#include <span>
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


        void bind_non_sparse(vk::Buffer desc);

        void bind_non_sparse(vk::Buffer desc, vk::MemoryPropertyFlags flags);

    private:
        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;

        // std::array<vk::MemoryType, VK_MAX_MEMORY_TYPES> _mem_types;
        // std::array<vk::MemoryHeap, VK_MAX_MEMORY_HEAPS> _mem_heaps;
        std::array<vk::UniqueDeviceMemory, VK_MAX_MEMORY_TYPES> _mem_pools;
    };

    global_allocator::global_allocator(const context& ctx)
        : _pdevice(ctx.physical_device), _ldevice(ctx.logical_device.get())
    {
        const auto properties = _pdevice.getMemoryProperties();
        for (uint32_t heap_index = 0; heap_index < properties.memoryHeapCount; ++heap_index)
        {
            const auto heap = properties.memoryHeaps[heap_index];

            const vk::MemoryAllocateInfo alloc_info{
                vk::DeviceSize{ heap.size / 2 }, // reserve half of heap capacity
                heap_index
            };
            _mem_pools[heap_index] = _ldevice.allocateMemoryUnique(alloc_info);
        }
    }

    void global_allocator::bind_non_sparse(vk::Buffer desc)
    {
        /*
        const auto buffer_mem_req = _ldevice.getBufferMemoryRequirements(desc);
        assert(buffer_mem_req.memoryTypeBits != 0, "Required by Vulkan standard");
        assert(buffer_mem_req.alignment % 2 == 0, "Required by Vulkan standard");

        for (uint32_t mem_type_index = 0; mem_type_index < sizeof(decltype(buffer_mem_req.memoryTypeBits)) * 4;)
        {
            while (buffer_mem_req.memoryTypeBits & (1 << mem_type_index))
                ++mem_type_index;

            auto&      mem_pool = _mem_pools[mem_type_index].get();
            const auto mem_bind = mem_pool.allocate(buffer_mem_req.size, buffer_mem_req.alignment);

            _ldevice.bindBufferMemory(desc, mem_bind.memory, mem_bind.offset);
        }
        */
    }


    // Allocator for buffers with frequent device (GPU) usage
    class device_local_allocator
    {
    public:
        explicit device_local_allocator(const context& ctx, size_t bytes, const std::span<uint32_t> queue_families);

        device_local_allocator(const device_local_allocator&) = delete;
        device_local_allocator& operator=(const device_local_allocator&) = delete;

        device_local_allocator(device_local_allocator&&) = delete;
        device_local_allocator operator=(device_local_allocator&&) = delete;


        [[nodiscard]] std::byte* allocate(vk::Buffer buffer);
        //std::byte* allocate(const std::vector<vk::Buffer>& buffers);

        void deallocate(std::byte*);

    private:
        const vk::Device       _device;
        vk::UniqueDeviceMemory _memory;
        vk::UniqueBuffer       _buffer;
    };

    device_local_allocator::device_local_allocator(
        const context& ctx, size_t bytes, const std::span<std::uint32_t> queue_families)
        : _device{ ctx.logical_device.get() }
    {
        assert(bytes > 0);

        // prototype of typical ubo use case
        const vk::BufferCreateInfo buffer_info{
            {},
            vk::DeviceSize{ bytes },
            vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::SharingMode::eConcurrent,
            static_cast<uint32_t>(std::size(queue_families)),
            queue_families.data()
        };
        _buffer = _device.createBufferUnique(buffer_info);

        const auto specs = _device.getBufferMemoryRequirements(_buffer.get());

        const vk::MemoryAllocateInfo device_alloc_info{
            specs.size,
        };
        _memory = _device.allocateMemoryUnique(device_alloc_info);
    }

    [[nodiscard]] std::byte* device_local_allocator::allocate(vk::Buffer buffer)
    {
        const auto specs = _device.getBufferMemoryRequirements(buffer);

        _device.bindBufferMemory(buffer, _memory.get(), 0);
    }


    // Allocator for buffers with frequent host (CPU) access
    class host_stack_allocator
    {
    public:
        explicit host_stack_allocator(const context&, const vk::MemoryAllocateInfo&);

        host_stack_allocator(const host_stack_allocator&) = delete;
        host_stack_allocator& operator=(const host_stack_allocator&) = delete;

        ~host_stack_allocator() noexcept;

        [[nodiscard]] void* allocate(vk::Buffer b) noexcept;

        void deallocate(void* ptr) noexcept;

    private:
        const vk::Device       _device;
        vk::UniqueDeviceMemory _memory;
        std::byte*             _buffer;
        size_t                 _offset;
    };




    class fixed_host_ubo
    {
    public:
        explicit fixed_host_ubo(
            const context& ctx, size_t size, host_mapped_allocator& a) noexcept;

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

#endif // !DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP