#pragma once
#ifndef DRAKO_VULKAN_MESH_TYPES_HPP
#define DRAKO_VULKAN_MESH_TYPES_HPP

#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/vulkan_vertex_buffer.hpp"

#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    class mesh
    {
    public:
        explicit mesh(const vk::Device d, const drako::mesh& m)
            : _v_buffer_size{ m.vertex_buffer().size_bytes() }
            , _i_buffer_size{ m.index_buffer().size_bytes() }
        {
            const vk::BufferCreateInfo vertex_buffer_info{
                vk::BufferCreateFlagBits{},
                m.vertex_buffer().size_bytes(),
                vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                vk::SharingMode::eExclusive,
                0, nullptr /* not needed for exclusive ownership */
            };
            _v_buffer = d.createBufferUnique(vertex_buffer_info);

            const vk::BufferCreateInfo index_buffer_info{
                vk::BufferCreateFlagBits{},
                m.index_buffer().size_bytes(),
                vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                vk::SharingMode::eExclusive,
                0, nullptr /* not needed for exclusive ownership */
            };
            _i_buffer = d.createBufferUnique(vertex_buffer_info);
        }

        void bind_vertex_buffer_memory() noexcept;

        void bind_index_buffer_memory() noexcept;

        [[nodiscard]] vk::Buffer vertex_buffer_handle() const noexcept { return _v_buffer.get(); }

        [[nodiscard]] vk::Buffer index_buffer_handle() const noexcept { return _i_buffer.get(); }

        [[nodiscard]] size_t vertex_buffer_size() const noexcept { return _v_buffer_size; }

        [[nodiscard]] size_t index_buffer_size() const noexcept { return _i_buffer_size; }

    private:
        vk::UniqueBuffer _v_buffer;
        vk::UniqueBuffer _i_buffer;
        const size_t     _v_buffer_size;
        const size_t     _i_buffer_size;
    };

} // namespace drako::gpx::vulkan

#endif // !DRAKO_VULKAN_MESH_TYPES_HPP