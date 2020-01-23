#pragma once
#ifndef DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP
#define DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include <array>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    struct vulkan_memory_requirement
    {
        vk::MemoryPropertyFlags flags;
        vk::DeviceSize          bytes;
    };


    class vulkan_memory_pool
    {
    public:
        struct allocation
        {
            vk::DeviceMemory memory;
            vk::DeviceSize   offset;
        };


        explicit vulkan_memory_pool(vk::UniqueDeviceMemory&& allocation) noexcept;

        allocation allocate(vk::DeviceSize size, vk::DeviceSize alignment) noexcept;

        void deallocate(vk::DeviceSize bind) noexcept;

    private:
        const vk::UniqueDeviceMemory _alloc;
    };

    vulkan_memory_pool::vulkan_memory_pool(vk::UniqueDeviceMemory&& allocation) noexcept
        : _alloc(std::move(allocation))
    {
        DRAKO_ASSERT(_alloc.get() != VK_NULL_HANDLE);
    }


    class vulkan_memory_allocator
    {
    public:
        explicit vulkan_memory_allocator(
            vk::PhysicalDevice                            pdevice,
            vk::Device                                    ldevice,
            const std::vector<vulkan_memory_requirement>& required) noexcept;

        void bind_non_sparse(vk::Buffer desc) noexcept;

        void bind_non_sparse(vk::Buffer desc, vk::MemoryPropertyFlags flags) noexcept;

    private:
        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;

        // std::array<vk::MemoryType, VK_MAX_MEMORY_TYPES> _mem_types;
        // std::array<vk::MemoryHeap, VK_MAX_MEMORY_HEAPS> _mem_heaps;
        std::array<vulkan_memory_pool, VK_MAX_MEMORY_TYPES> _mem_pools;
    };


    vulkan_memory_allocator::vulkan_memory_allocator(
        vk::PhysicalDevice                            pdevice,
        vk::Device                                    ldevice,
        const std::vector<vulkan_memory_requirement>& required) noexcept
        : _pdevice(pdevice), _ldevice(ldevice)
    {
        DRAKO_ASSERT(pdevice != VK_NULL_HANDLE);
        DRAKO_ASSERT(ldevice != VK_NULL_HANDLE);

        const auto device_mem_properties = _pdevice.getMemoryProperties();
        for (uint32_t i = 0; i < device_mem_properties.memoryHeapCount; ++i)
        {
            const auto heap = device_mem_properties.memoryHeaps[i];

            const vk::MemoryAllocateInfo alloc_info{
                vk::DeviceSize{heap.size / 2}, // reserve half of heap capacity
                heap.};
            if (auto [err, mem] = _ldevice.allocateMemoryUnique(alloc_info);
                err != vk::Result::eSuccess)
            {
                std::exit(EXIT_FAILURE);
            }
            else
            {
            }
        }
    }

    void vulkan_memory_allocator::bind_non_sparse(vk::Buffer desc) noexcept
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

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_MEMORY_ALLOCATOR_HPP