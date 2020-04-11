#pragma once
#ifndef DRAKO_VULKAN_MESH_TYPES_HPP
#define DRAKO_VULKAN_MESH_TYPES_HPP

#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/vulkan_resource_loader.hpp"
#include "drako/graphics/vulkan_vertex_buffer.hpp"

#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    class vulkan_mesh_view
    {
    public:
        explicit vulkan_mesh_view(vk::Buffer vertices, size_t vcount,
            vk::Buffer indexes, size_t icount) noexcept
            : _v_buffer{ vertices }
            , _i_buffer{ indexes }
            , _v_buffer_size{ vcount }
            , _i_buffer_size{ icount }
        {
        }

        [[nodiscard]] vk::Buffer vertex_buffer_handle() const noexcept { return _v_buffer; }

        [[nodiscard]] vk::Buffer index_buffer_handle() const noexcept { return _i_buffer; }

        [[nodiscard]] size_t vertex_buffer_size() const noexcept { return _v_buffer_size; }

        [[nodiscard]] size_t index_buffer_size() const noexcept { return _i_buffer_size; }

    private:
        vk::Buffer   _v_buffer;
        vk::Buffer   _i_buffer;
        const size_t _v_buffer_size;
        const size_t _i_buffer_size;
    };


    template <typename VulkanAlloc>
    class vulkan_mesh
    {
    public:
        using VkAl = VulkanAlloc;

        explicit vulkan_mesh(vk::Device d, mesh_view m, VkAl& al) noexcept
            : _vertices(d, m.vertex_data_size(), m.vertex_data(), al)
            , _indexes(d, m.index_data_size(), m.index_data_size(), al)
        {
        }

        explicit vulkan_mesh(vk::Device d, mesh_view m, VkAl& al, vulkan_resource_loader& loader) noexcept
            : _vertices(d, m.vertex_data_size(), m.vertex_data(), al, loader)
            , _indexes(d, m.index_data_size(), m.index_data_size(), al, loader)
        {
        }

        template <typename Alloc>
        explicit vulkan_mesh(vk::Device d, const mesh<Alloc>& m, VkAl& al) noexcept
            : _vertices(d, std::size(m.vertex_buffer()), m.vertex_buffer(), al)
            , _indexes(d, std::size(m.index_buffer()), m.index_buffer(), al)
        {
        }

        template <typename Alloc>
        explicit vulkan_mesh(vk::Device d, const mesh<Alloc>& m, VkAl& al, vulkan_resource_loader& loader) noexcept
            : _vertices(d, std::size(m.vertex_buffer()), m.vertex_buffer(), al, loader)
            , _indexes(d, std::size(m.index_buffer()), m.index_buffer(), al, loader)
        {
        }

        vulkan_mesh(const vulkan_mesh&) = delete;
        vulkan_mesh& operator=(const vulkan_mesh&) = delete;

        vulkan_mesh(vulkan_mesh&&) = delete;
        vulkan_mesh& operator=(vulkan_mesh&&) = delete;

        [[nodiscard]] explicit operator vulkan_mesh_view() const noexcept
        {
            return vulkan_mesh_view{ _vertices.buffer(), _vertices.size(), _indexes.buffer(), _indexes.size() };
        }

        [[nodiscard]] constexpr const auto&
        index_buffer() const noexcept { return _vertex_buffer; }

        [[nodiscard]] constexpr const auto
        index_buffer_size() const noexcept { return _index_buffer_size; }

        [[nodiscard]] constexpr const auto&
        vertex_buffer() const noexcept { return _index_buffer; }

        [[nodiscard]] constexpr auto
        vertex_buffer_size() const noexcept { return _vertex_buffer_size; }

    private:
        //const vk::Device _ldevice;
        //vk::Buffer       _vertex_buffer;
        //vk::DeviceSize   _vertex_buffer_size;
        //vk::Buffer       _index_buffer;
        //vk::DeviceSize   _index_buffer_size;

        vulkan_vertex_buffer<VkAl> _vertices;
        vulkan_index_buffer<VkAl>  _indexes;
    };

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_MESH_TYPES_HPP