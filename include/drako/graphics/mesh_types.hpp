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


    // Structure specifying format and layout of the data stored by a mesh asset.
    struct MeshMetaInfo
    {
        // Primitive topology of the mesh.
        enum class Topology : std::uint8_t
        {
            undefined = 0,
            triangle_list,
            triangle_strip,
            triangle_fan
        };

        // Number of vertices in the mesh vertex buffer.
        std::uint32_t vertex_count = 0;

        // Number of indices in the mesh index buffer.
        std::uint32_t index_count = 0;

        // Byte size of a single vertex element.
        std::uint8_t vertex_size_bytes = 0;

        // Byte size of a single index element.
        std::uint8_t index_size_bytes = 0;

        // Topology of the mesh.
        Topology topology = Topology::undefined;
    };

    class MeshView
    {
    public:
        explicit constexpr MeshView() noexcept = default;

        explicit constexpr MeshView(
            std::span<const std::byte> verts,
            std::span<const std::byte> index) noexcept
            : _verts{ verts }, _index{ index } {}

        MeshView(const MeshView&) noexcept = default;
        MeshView& operator=(const MeshView&) noexcept = default;

        [[nodiscard]] std::span<const std::byte> vertex_buffer() const noexcept { return _verts; }

        [[nodiscard]] std::span<const std::byte> index_buffer() const noexcept { return _index; }

    private:
        std::span<const std::byte> _verts; // mesh vertices raw data
        std::span<const std::byte> _index; // mesh indices raw data
    };



    /// @brief Mesh asset.
    class Mesh
    {
    public:
        explicit constexpr Mesh() noexcept = default;

        explicit Mesh(const MeshMetaInfo& meta,
            std::span<const std::byte>    verts,
            std::span<const std::byte>    index) noexcept
            : _meta{ meta }
            , _verts{ std::cbegin(verts), std::cend(verts) }
            , _index{ std::cbegin(index), std::cend(index) }
        {}

        Mesh(const Mesh&) noexcept = default;
        Mesh& operator=(const Mesh&) noexcept = default;

        [[nodiscard]] explicit operator MeshView() const noexcept
        {
            return MeshView{ vertex_buffer(), index_buffer() };
        }

        //[[nodiscard]] std::span<const std::byte> vertex_buffer() noexcept { return _verts; }
        [[nodiscard]] std::span<const std::byte> vertex_buffer() const noexcept { return _verts; }

        //[[nodiscard]] std::span<const std::byte> index_buffer() noexcept { return _index; }
        [[nodiscard]] std::span<const std::byte> index_buffer() const noexcept { return _index; }

    private:
        std::vector<std::byte> _verts; // mesh vertices raw data
        std::vector<std::byte> _index; // mesh indices raw data
        MeshMetaInfo           _meta;
    };

} // namespace drako

#endif // !DRAKO_MESH_TYPES_HPP