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

namespace drako::vulkan
{
    class GlobalAllocator
    {
    public:
        explicit GlobalAllocator(const Context& ctx);

        GlobalAllocator(const GlobalAllocator&) = delete;
        GlobalAllocator& operator=(const GlobalAllocator&) = delete;

        GlobalAllocator(GlobalAllocator&&) = delete;
        GlobalAllocator& operator=(GlobalAllocator&&) = delete;

        void bind_non_sparse(vk::Buffer desc);

        void bind_non_sparse(vk::Buffer desc, vk::MemoryPropertyFlags flags);

    private:
        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;

        // std::array<vk::MemoryType, VK_MAX_MEMORY_TYPES> _mem_types;
        // std::array<vk::MemoryHeap, VK_MAX_MEMORY_HEAPS> _mem_heaps;
        std::array<vk::UniqueDeviceMemory, VK_MAX_MEMORY_TYPES> _mem_pools;
    };

    GlobalAllocator::GlobalAllocator(const Context& ctx)
        : _pdevice(ctx.physical_device), _ldevice(ctx.logical_device.get())
    {
        const auto properties = _pdevice.getMemoryProperties();
        for (uint32_t heap_index = 0; heap_index < properties.memoryHeapCount; ++heap_index)
        {
            const auto& heap = properties.memoryHeaps[heap_index];

            const vk::MemoryAllocateInfo alloc_info{
                vk::DeviceSize{ heap.size / 2 }, // reserve half of heap capacity
                heap_index
            };
            _mem_pools[heap_index] = _ldevice.allocateMemoryUnique(alloc_info);
        }
    }

    void GlobalAllocator::bind_non_sparse(vk::Buffer desc)
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

} // namespace drako::vulkan

#endif // !DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP