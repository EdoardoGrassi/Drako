#pragma once
#ifndef DRAKO_VULKAN_VERTEX_BUFFER_HPP
#define DRAKO_VULKAN_VERTEX_BUFFER_HPP

#include "drako/devel/logging.hpp"

#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    // Uniform buffer on gpu side specialized for vertex attributes.
    class vk_vertex_buffer
    {
    public:
        explicit vk_vertex_buffer(const vk::Device ldevice,
            vk::Queue                              transfert_queue,
            vk::DeviceSize                         size,
            const void*                            data,
            vk::CommandBuffer                      transfert_cmdbuffer) noexcept
            : _ldevice(ldevice)
            , _size(size)
        {
            const vk::BufferCreateInfo staging_buffer_info{
                vk::BufferCreateFlagBits::eSparseBinding,
                size,
                vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc,
                vk::SharingMode::eExclusive,
                0, nullptr /* Not needed for exclusive ownership */
            };

            auto [staging_buffer_result, staging_buffer] = _ldevice.createBuffer(staging_buffer_info);
            if (staging_buffer_result != vk::Result::eSuccess)
            {
                DRAKO_LOG_FAILURE("[Vulkan] Failed to create staging buffer - " + to_string(staging_buffer_result));
                std::exit(EXIT_FAILURE);
            }

            const vk::BufferCreateInfo vertex_buffer_info{
                vk::BufferCreateFlagBits::eSparseBinding,
                size,
                vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                vk::SharingMode::eExclusive,
                0, nullptr /* Not needed for exclusive ownership */
            };

            auto [vertex_buffer_result, vertex_buffer] = _ldevice.createBuffer(vertex_buffer_info);
            if (vertex_buffer_result != vk::Result::eSuccess)
            {
                DRAKO_LOG_FAILURE("[Vulkan] Failed to create vertex buffer - " + to_string(vertex_buffer_result));
                std::exit(EXIT_FAILURE);
            }

            const vk::MemoryAllocateInfo allocation{
                size,
                0
            };
            const auto [allocate_result, staging_memory] = _ldevice.allocateMemory(allocation);
            if (allocate_result != vk::Result::eSuccess)
            {
                std::exit(EXIT_FAILURE);
            }

            const auto [mapping_result, memory_ptr] = _ldevice.mapMemory(
                staging_memory,
                0,
                size,
                vk::MemoryMapFlags{});
            if (mapping_result != vk::Result::eSuccess)
            {
                std::exit(EXIT_FAILURE);
            }

            std::memcpy(memory_ptr, data, size);
            _ldevice.unmapMemory(staging_memory);
            _ldevice.freeMemory(staging_memory);


            const vk::CommandBufferBeginInfo transfert_begin_info(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
                nullptr);
            transfert_cmdbuffer.begin(transfert_begin_info);

            const vk::BufferCopy copy_region(0, 0, size);
            transfert_cmdbuffer.copyBuffer(staging_buffer, vertex_buffer, 1, &copy_region);

            transfert_cmdbuffer.end();

            const vk::SubmitInfo submit_info({}, {}, {}, 1, &transfert_cmdbuffer, {}, {});
            transfert_queue.submit(1, &submit_info, vk::Fence{});
            transfert_queue.waitIdle();


            _buffer = vertex_buffer;
        }

        ~vk_vertex_buffer()
        {
            _ldevice.destroyBuffer(_buffer);
        }

        [[nodiscard]] constexpr vk::DeviceSize count() const noexcept { return _size; }

        [[nodiscard]] constexpr const vk::Buffer& handle() const noexcept { return _buffer; }

    private:
        const vk::DeviceSize _size;
        const vk::Device     _ldevice;
        vk::Buffer           _buffer;
    };



    template <typename VulkanAllocator>
    class vulkan_vertex_buffer
    {
    public:
        using VkAl = VulkanAllocator;

        explicit vulkan_vertex_buffer(vk::Device device, size_t size, const void* data, VkAl& alloc) noexcept;

        [[nodiscard]] constexpr vk::Buffer
        buffer() const noexcept { return _buffer.get(); }

        [[nodiscard]] constexpr vk::DeviceSize
        size() const noexcept { return _size; }

    private:
        const vk::Device     _device;
        const vk::DeviceSize _size;
        vk::UniqueBuffer     _buffer;
        VkAl&                _alloc;
    };

    template <typename VkAl>
    vulkan_vertex_buffer<VkAl>::vulkan_vertex_buffer(vk::Device device, size_t size, const void* data, VkAl& alloc) noexcept
        : _device(device)
        , _size(size)
    {
        const vk::BufferCreateInfo buffer_info{
            vk::BufferCreateFlagBits{},
            _size,
            vk::BufferUsageFlagBits::eVertexBuffer,
            vk::SharingMode::eExclusive,
            0, nullptr /* Not needed for exclusive ownership */
        };

        if (auto [err, buffer] = _device.createBufferUnique(buffer_info);
            err == vk::Result::eSuccess)
        {
            _buffer = std::move(buffer);
        }
        else
        {
            std::exit(EXIT_FAILURE);
        }

        const auto memory = alloc.allocate(_buffer.get(), vk::MemoryPropertyFlagBits::eHostVisible);

        if (auto [err, dst] = _device.mapMemory(memory.memory, memory.offset, memory.size, vk::MemoryMapFlags{});
            err == vk::Result::eSuccess)
        {
            std::memcpy(dst, data, _size);

            const vk::MappedMemoryRange buffer_range[] = {
                memory.memory,
                memory.offset,
                memory.size
            };

            _device.flushMappedMemoryRanges(std::size(buffer_range), buffer_range);
            _device.unmapMemory(memory.memory);
        }
        else
        {
            std::exit(EXIT_FAILURE);
        }
    }


    // Uniform buffer on gpu side specilized for vertex indexes.
    class vk_index_buffer
    {
    public:
        explicit vk_index_buffer(const vk::Device ldevice_,
            uint32_t                              queue_index_,
            vk::Queue                             transfert_queue_,
            vk::DeviceSize                        buffer_size_,
            const void*                           data_,
            vk::CommandBuffer                     transfert_cmdbuffer_) noexcept
            : _ldevice(ldevice_)
            , _size(buffer_size_)
        {
            const vk::BufferCreateInfo staging_buffer_info(
                vk::BufferCreateFlagBits::eSparseBinding,
                buffer_size_,
                vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferSrc,
                vk::SharingMode::eExclusive,
                0, nullptr);

            auto [staging_buffer_create_result, staging_buffer] = _ldevice.createBuffer(staging_buffer_info);
            if (staging_buffer_create_result != vk::Result::eSuccess)
            {
                std::exit(EXIT_FAILURE);
            }

            const vk::BufferCreateInfo vertex_buffer_info(
                vk::BufferCreateFlagBits::eSparseBinding,
                buffer_size_,
                vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                vk::SharingMode::eExclusive,
                0, nullptr);

            auto [vertex_buffer_create_result, vertex_buffer] = _ldevice.createBuffer(vertex_buffer_info);
            if (vertex_buffer_create_result != vk::Result::eSuccess)
            {
                std::exit(EXIT_FAILURE);
            }

            vk::DeviceMemory staging_memory;
            const auto [map_result, ptr] = _ldevice.mapMemory(
                staging_memory,
                0,
                buffer_size_,
                {});

            std::memcpy(ptr, data_, buffer_size_);
            _ldevice.unmapMemory(staging_memory);


            const vk::CommandBufferBeginInfo transfert_begin_info{
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
                nullptr /* no buffer inheritance */
            };
            transfert_cmdbuffer_.begin(transfert_begin_info);

            const vk::BufferCopy regions[] = { { 0, 0, buffer_size_ } };
            transfert_cmdbuffer_.copyBuffer(staging_buffer, vertex_buffer,
                static_cast<uint32_t>(std::size(regions)), regions);

            transfert_cmdbuffer_.end();

            const vk::SubmitInfo submits[] = {
                {
                    0, nullptr, nullptr, // before execution synchronization not needed
                    1, &transfert_cmdbuffer_,
                    0, nullptr // after execution synchronization not needed
                }
            };
            transfert_queue_.submit(static_cast<uint32_t>(std::size(submits)), submits, vk::Fence{ nullptr });
            transfert_queue_.waitIdle(); // syncronize with finished queue operation

            _buffer = vertex_buffer;
        }

        ~vk_index_buffer()
        {
            _ldevice.destroyBuffer(_buffer);
        }

        [[nodiscard]] constexpr vk::DeviceSize count() const noexcept { return _size; }

        [[nodiscard]] constexpr const vk::Buffer& handle() const noexcept { return _buffer; }

    private:
        const vk::DeviceSize _size;
        const vk::Device     _ldevice;
        vk::Buffer           _buffer;
    };


    template <typename VulkanAllocator>
    class vulkan_index_buffer
    {
    public:
        using VkAl = VulkanAllocator;

        explicit vulkan_index_buffer(vk::Device device, size_t size, const void* data, VkAl& alloc) noexcept;

        [[nodiscard]] constexpr vk::Buffer
        buffer() const noexcept { return _buffer.get(); }

        [[nodiscard]] constexpr size_t
        size() const noexcept { return _size; }

    private:
        const vk::Device     _device;
        const vk::DeviceSize _size;
        vk::UniqueBuffer     _buffer;
        VkAl&                _alloc;
    };

    template <typename VkAl>
    vulkan_index_buffer<VkAl>::vulkan_index_buffer(vk::Device device, size_t size, const void* data, VkAl& alloc) noexcept
        : _device(device)
        , _size(size)
    {
        const vk::BufferCreateInfo buffer_info{
            vk::BufferCreateFlagBits{},
            _size,
            vk::BufferUsageFlagBits::eIndexBuffer,
            vk::SharingMode::eExclusive,
            0, nullptr /* Not needed for exclusive ownership */
        };

        if (auto [err, buffer] = _device.createBufferUnique(buffer_info);
            err == vk::Result::eSuccess)
        {
            _buffer = std::move(buffer);
        }
        else
        {
            std::exit(EXIT_FAILURE);
        }

        const auto memory = alloc.allocate(_buffer.get(), vk::MemoryPropertyFlagBits::eHostVisible);

        if (auto [err, dst] = _device.mapMemory(memory.memory, memory.offset, memory.size, vk::MemoryMapFlags{});
            err == vk::Result::eSuccess)
        {
            std::memcpy(dst, data, _size);

            const vk::MappedMemoryRange buffer_range[] = {
                memory.memory,
                memory.offset,
                memory.size
            };

            _device.flushMappedMemoryRanges(std::size(buffer_range), buffer_range);
            _device.unmapMemory(memory.memory);
        }
        else
        {
            std::exit(EXIT_FAILURE);
        }
    }

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_VERTEX_BUFFER_HPP