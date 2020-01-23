#pragma once
#ifndef DRAKO_VULKAN_RESOURCE_LOADER_HPP
#define DRAKO_VULKAN_RESOURCE_LOADER_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"

#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_queue.hpp"
#include "drako/graphics/vulkan_texture_types.hpp"

#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    class vulkan_resource_loader
    {
        using vk_queue_index       = std::uint32_t;
        using vk_queue_family      = std::uint32_t;
        using vk_memory_type_index = std::uint32_t;

    public:
        vulkan_resource_loader(vk::PhysicalDevice pdevice, vk::Device ldevice, const vulkan_transfer_queue& worker, size_t capacity) noexcept;
        ~vulkan_resource_loader() noexcept;

        // template <typename Vertex, typename Index>
        // DRAKO_NODISCARD vulkan_mesh load(const basic_mesh_asset<Vertex, Index>& mesh) noexcept;

    private:
        const vk::PhysicalDevice _pdevice;
        const vk::Device         _ldevice;
        vk::UniqueBuffer         _staging_buffer;
        vk::UniqueDeviceMemory   _staging_memory;

        vk::UniqueCommandPool       _worker_command_pool;
        vk::UniqueCommandBuffer     _worker_command_buffer;
        const vulkan_transfer_queue _worker_queue;

        const vk_memory_type_index _vertex_buffer_memory;
        const vk_memory_type_index _index_buffer_memory;
    };

    vulkan_resource_loader::vulkan_resource_loader(
        vk::PhysicalDevice           pdevice,
        vk::Device                   ldevice,
        const vulkan_transfer_queue& worker,
        size_t                       capacity) noexcept
        : _pdevice(pdevice)
        , _ldevice(ldevice)
        , _worker_queue(worker)
    {
        DRAKO_ASSERT(pdevice != vk::PhysicalDevice{ nullptr });
        DRAKO_ASSERT(ldevice != vk::Device{ nullptr });
        DRAKO_ASSERT(capacity > 0);

        const vk::BufferCreateInfo staging_buffer_info{
            vk::BufferCreateFlagBits::eSparseBinding,
            static_cast<uint32_t>(capacity),
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::SharingMode::eExclusive,
            0, nullptr /* Not needed for exclusive ownership */
        };
        if (auto [err, buffer] = _ldevice.createBufferUnique(staging_buffer_info);
            DRAKO_UNLIKELY(err != vk::Result::eSuccess))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            _staging_buffer = std::move(buffer);
        }

        const auto mem_loader_requirements = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        const auto mem_buffer_requirements = _ldevice.getBufferMemoryRequirements(_staging_buffer.get());
        const auto mem_properties          = _pdevice.getMemoryProperties();
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i)
        {
            if ((mem_buffer_requirements.memoryTypeBits & (1 << i)) &&
                (mem_properties.memoryTypes[i].propertyFlags & mem_loader_requirements))
                break;
        }

        const vk::MemoryAllocateInfo alloc_info{
            vk::DeviceSize{ capacity },
            i
        };
        if (auto [err, memory] = _ldevice.allocateMemoryUnique(alloc_info);
            err != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            _staging_memory = std::move(memory);
        }

        if (auto err = _ldevice.bindBufferMemory(_staging_buffer.get(), _staging_memory.get(), vk::DeviceSize{ 0 });
            err != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }

        const vk::CommandPoolCreateInfo command_pool_create_info{
            vk::CommandPoolCreateFlags{},
            _worker_queue.family
        };
        if (auto [err, pool] = _ldevice.createCommandPoolUnique(command_pool_create_info);
            err != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            _worker_command_pool = std::move(pool);
        }

        const vk::CommandBufferAllocateInfo command_buffer_alloc_info{
            _worker_command_pool.get(),
            vk::CommandBufferLevel::ePrimary,
            1 // command buffers count
        };
        if (auto [err, buffer] = _ldevice.allocateCommandBuffersUnique(command_buffer_alloc_info);
            DRAKO_UNLIKELY(err != vk::Result::eSuccess))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            _worker_command_buffer = std::move(buffer[0]);
        }
    }


    /*
    template <typename Vertex, typename Index>
    inline vulkan_mesh vulkan_resource_loader::load(const basic_mesh_asset<Vertex, Index>& mesh) noexcept
    {
        using mesh_type = std::remove_reference_t<std::remove_cv_t<decltype(mesh)>>;

        const vk::DeviceSize vertex_buffer_size{std::size(mesh.vertex_buffer()) * sizeof(mesh_type::vertex_type)};
        const vk::DeviceSize index_buffer_size{std::size(mesh.index_buffer()) * sizeof(mesh_type::index_type)};

        const vk::MemoryAllocateInfo vertex_buffer_alloc_info{
            vertex_buffer_size,
            _vertex_buffer_memory};
        if (auto [err, mem] _ldevice.allocateMemory(vertex_buffer_alloc_info);
            DRAKO_UNLIKELY(err != vk::Result::eSuccess))
        {
            std::exit(EXIT_FAILURE);
        }

        const vk::BufferCreateInfo vertex_buffer_info{
            vk::BufferCreateFlagBits{},
            mesh.vertex_buffer().size() * sizeof(VertexFormat),
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::SharingMode::eExclusive,
            0, nullptr // not needed for exclusive ownership
        };

        if (const auto [err, dst] = _ldevice.mapMemory(_staging_memory, vk::DeviceSize{0}, vertex_buffer_size, vk::MemoryMapFlags{});
            DRAKO_UNLIKELY(err != vk::Result::eSuccess))
        {
            std::exit(EXIT_FAILURE);
        }
        else
        {
            const auto src = mesh.vertex_buffer().data();
            std::memcpy(dst, src, vertex_buffer_size);
            _ldevice.unmapMemory(_staging_memory);
        }

        const vk::CommandBufferBeginInfo transfert_begin_info{
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
            nullptr};

        if (_worker_command_buffer.get().begin(transfert_begin_info) != vk::Result::eSuccess)
            std::exit(EXIT_FAILURE);

        const vk::BufferCopy region{
            vk::DeviceSize{0}, // src offset
            vk::DeviceSize{0}, // dst offset
            vertex_buffer_size};
        _worker_command_buffer.get().copyBuffer(_staging_buffer, vertex_buffer, 1, &regions);

        if (_worker_command_buffer.get().end() != vk::Result::eSuccess)
            std::exit(EXIT_FAILURE);

        const vk::SubmitInfo transfert_submit_info{
            0, nullptr, nullptr,
            1, &_worker_command_buffer,
            0, nullptr};
        if (_worker_queue.submit(transfert_submit_info != vk::Result::eSuccess);
            std::exit(EXIT_FAILURE);
        _worker_queue.waitIdle();

        return vulkan_mesh{}; // TODO: create proper object
    }
    */

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_RESOURCE_LOADER_HPP