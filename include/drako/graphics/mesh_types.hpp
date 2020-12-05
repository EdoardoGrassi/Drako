#pragma once
#ifndef DRAKO_MESH_TYPES_HPP
#define DRAKO_MESH_TYPES_HPP

#include "drako/core/encoding.hpp"

#include <cstddef>
#include <iostream>
#include <span>
#include <string>
#include <type_traits>

namespace drako
{
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
            meta::format f, vertex_attribute va, std::uint8_t dimension) noexcept
            : format(f), attribute(va), dimension(dimension) {}
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

    class mesh_view
    {
    public:
        explicit constexpr mesh_view() noexcept = default;

        explicit constexpr mesh_view(
            std::span<const std::byte> verts,
            std::span<const std::byte> index) noexcept
            : _verts{ verts }, _index{ index } {}

        mesh_view(const mesh_view&) noexcept = default;
        mesh_view& operator=(const mesh_view&) noexcept = default;

        [[nodiscard]] std::span<const std::byte> vertex_buffer() const noexcept { return _verts; }

        [[nodiscard]] std::span<const std::byte> index_buffer() const noexcept { return _index; }

    private:
        std::span<const std::byte> _verts; // mesh vertices raw data
        std::span<const std::byte> _index; // mesh indices raw data
    };



    /// @brief Mesh asset.
    class mesh
    {
    public:
        explicit constexpr mesh() noexcept = default;

        explicit mesh(const mesh_desc_t& meta,
            std::span<const std::byte>   verts,
            std::span<const std::byte>   index) noexcept
            : _meta{ meta }, _verts{ verts }, _index{ index } {}

        mesh(const mesh&) noexcept = default;
        mesh& operator=(const mesh&) noexcept = default;

        /*
        [[nodiscard]] explicit operator mesh_view() const noexcept
        {
            return mesh_view{ vertex_buffer(), index_buffer() };
        }
        */

        //[[nodiscard]] std::span<const std::byte> vertex_buffer() noexcept { return _verts; }
        [[nodiscard]] std::span<const std::byte> vertex_buffer() const noexcept { return _verts; }

        //[[nodiscard]] std::span<const std::byte> index_buffer() noexcept { return _index; }
        [[nodiscard]] std::span<const std::byte> index_buffer() const noexcept { return _index; }

    private:
        mesh_desc_t                _meta;
        std::span<const std::byte> _verts; // mesh vertices raw data
        std::span<const std::byte> _index; // mesh indices raw data
    };

} // namespace drako

#endif // !DRAKO_MESH_TYPES_HPP