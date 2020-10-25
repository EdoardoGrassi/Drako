#pragma once
#ifndef DRAKO_MESH_TYPES_HPP_
#define DRAKO_MESH_TYPES_HPP_

#include "drako/core/meta/encoding.hpp"
#include "drako/system/memory_stream.hpp"

#include <cstddef>
#include <iostream>
#include <span>
#include <type_traits>

namespace drako
{
    // Uniquely identifies a mesh resource.
    enum class mesh_id : std::uint32_t;

    [[nodiscard]] std::string to_string(mesh_id id)
    {
        return std::to_string(static_cast<std::underlying_type_t<mesh_id>>(id));
    }


    enum class vertex_attribute : std::uint8_t
    {
        position,
        normal,
        color,
        index
    };

    // Metadata with memory and semantic information.
    struct vertex_attribute_descriptor
    { //^^^ template <const char* Name, meta::format Format, size_t Count>
        meta::format     format;
        vertex_attribute attribute;
        std::uint8_t     dimension;

        constexpr vertex_attribute_descriptor(
            meta::format     format,
            vertex_attribute attribute,
            std::uint8_t     dimension) noexcept
            : format(format)
            , attribute(attribute)
            , dimension(dimension)
        {
        }
    };


    // Primitive topology of the mesh.
    enum class mesh_topology : std::uint8_t
    {
        undefined = 0,
        triangle_list,
        triangle_strip,
        triangle_fan
    };

    // Structure specifying format and layout of the data stored by a mesh asset.
    struct mesh_desc_t
    {
    public:
        explicit constexpr mesh_desc_t(
            std::uint32_t vertex_count,
            std::uint8_t  vertex_stride_bytes,
            std::uint32_t index_count,
            std::uint8_t  index_stride_bytes,
            mesh_topology topology) noexcept
            : vertex_count{ vertex_count }
            , index_count{ index_count }
            , vertex_size_bytes{ vertex_stride_bytes }
            , index_size_bytes{ index_stride_bytes }
            , topology{ topology }
        {
        }

        // Number of vertices in the mesh vertex buffer.
        std::uint32_t vertex_count = 0;

        // Number of indices in the mesh index buffer.
        std::uint32_t index_count = 0;

        // Byte size of a single vertex element.
        std::uint8_t vertex_size_bytes = 0;

        // Byte size of a single index element.
        std::uint8_t index_size_bytes = 0;

        // Topology of the mesh.
        mesh_topology topology = mesh_topology::undefined;
    };

    [[nodiscard]] size_t vertex_buffer_size_bytes(const mesh_desc_t& desc) noexcept
    {
        return static_cast<size_t>(desc.vertex_count) * static_cast<size_t>(desc.vertex_size_bytes);
    }

    [[nodiscard]] size_t index_buffer_size_bytes(const mesh_desc_t& desc) noexcept
    {
        return static_cast<size_t>(desc.index_count) * static_cast<size_t>(desc.index_size_bytes);
    }


    class mesh_view
    {
    public:
        explicit constexpr mesh_view() noexcept = default;

        explicit constexpr mesh_view(
            const std::span<std::byte> vertex_data,
            const std::span<std::byte> index_data) noexcept
            : _verts{ vertex_data }, _index{ index_data }
        {
        }

        mesh_view(const mesh_view&) noexcept = default;
        mesh_view& operator=(const mesh_view&) noexcept = default;

        [[nodiscard]] const std::span<std::byte> vertex_buffer() const noexcept { return _verts; }

        [[nodiscard]] const std::span<std::byte> index_buffer() const noexcept { return _index; }

    private:
        //const std::byte* _verts_data;
        //const std::byte* _index_data;
        //const size_t     _verts_size;
        //const size_t     _index_size;

        std::span<std::byte> _verts;
        std::span<std::byte> _index;
    };



    // Mesh asset.
    class mesh
    {
    public:
        explicit constexpr mesh() noexcept
            : _meta{ 0, 0, 0, 0, mesh_topology::undefined }
            , _verts{ nullptr }
            , _index{ nullptr }
        {
        }

        explicit mesh(const mesh_desc_t& meta,
            const std::span<std::byte>   vertex_data,
            const std::span<std::byte>   index_data) noexcept
            : _meta{ meta }
            , _verts{ vertex_data.data() }
            , _index{ index_data.data() }
        {
        }

        mesh(const mesh&) = delete;
        mesh& operator=(const mesh&) = delete;

        constexpr mesh(mesh&& other) noexcept;
        constexpr mesh& operator=(mesh&& other) noexcept;

        [[nodiscard]] explicit operator mesh_view() const noexcept
        {
            return mesh_view{ vertex_buffer(), index_buffer() };
        }

        [[nodiscard]] std::span<std::byte>       vertex_buffer() noexcept;
        [[nodiscard]] const std::span<std::byte> vertex_buffer() const noexcept;

        [[nodiscard]] std::span<std::byte>       index_buffer() noexcept;
        [[nodiscard]] const std::span<std::byte> index_buffer() const noexcept;

    private:
        mesh_desc_t _meta;
        std::byte*  _verts;
        std::byte*  _index;
    };


    std::span<std::byte> mesh::vertex_buffer() noexcept
    {
        return { _verts, vertex_buffer_size_bytes(_meta) };
    }

    const std::span<std::byte> mesh::vertex_buffer() const noexcept
    {
        return { _verts, vertex_buffer_size_bytes(_meta) };
    }

    std::span<std::byte> mesh::index_buffer() noexcept
    {
        return { _index, index_buffer_size_bytes(_meta) };
    }

    const std::span<std::byte> mesh::index_buffer() const noexcept
    {
        return { _index, index_buffer_size_bytes(_meta) };
    }

} // namespace drako

#endif // !DRAKO_MESH_TYPES_HPP