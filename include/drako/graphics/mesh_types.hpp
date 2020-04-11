#pragma once
#ifndef DRAKO_MESH_TYPES_HPP_
#define DRAKO_MESH_TYPES_HPP_

#include "drako/core/meta/encoding.hpp"
#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/system/memory_stream.hpp"

#include <cstddef>
#include <iostream>
#include <type_traits>
#include <vector>

namespace drako
{
    // Uniquely identifies a mesh resource.
    enum class mesh_id : std::uint32_t;

    [[nodiscard]] DRAKO_FORCE_INLINE std::string to_string(mesh_id id)
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
    static_assert(std::is_trivially_copy_constructible_v<mesh_desc_t>,
        "Required for direct serialization with memcpy");

    [[nodiscard]] DRAKO_FORCE_INLINE size_t
    vertex_buffer_size_bytes(const mesh_desc_t& desc) noexcept
    {
        return desc.vertex_count * desc.vertex_size_bytes;
    }

    [[nodiscard]] DRAKO_FORCE_INLINE size_t
    index_buffer_size_bytes(const mesh_desc_t& desc) noexcept
    {
        return desc.index_count * desc.index_size_bytes;
    }


    class mesh_view
    {
    public:
        explicit constexpr mesh_view() noexcept
            : _verts_data{ nullptr }
            , _index_data{ nullptr }
            , _verts_size{ 0 }
            , _index_size{ 0 }
        {
        }

        explicit mesh_view(const std::byte* verts_data, size_t verts_bytes,
            const std::byte* index_data, size_t index_bytes) noexcept
            : _verts_data{ verts_data }
            , _index_data{ index_data }
            , _verts_size{ verts_bytes }
            , _index_size{ index_bytes }
        {
            DRAKO_ASSERT(verts_data != nullptr);
            DRAKO_ASSERT(index_data != nullptr);
            DRAKO_ASSERT(verts_bytes != 0);
            DRAKO_ASSERT(index_bytes != 0);
        }

        mesh_view(const mesh_view&) noexcept = default;
        mesh_view& operator=(const mesh_view&) noexcept = default;

        [[nodiscard]] DRAKO_FORCE_INLINE const std::byte*
        vertex_data() const noexcept { return _verts_data; }

        [[nodiscard]] DRAKO_FORCE_INLINE const std::byte*
        index_data() const noexcept { return _index_data; }

        [[nodiscard]] DRAKO_FORCE_INLINE constexpr size_t
        vertex_data_size() const noexcept { return _verts_size; }

        [[nodiscard]] DRAKO_FORCE_INLINE constexpr size_t
        index_data_size() const noexcept { return _index_size; }

    private:
        const std::byte* _verts_data;
        const std::byte* _index_data;
        const size_t     _verts_size;
        const size_t     _index_size;
    };



    // Mesh asset.
    template <typename Alloc = std::allocator<std::byte>>
    class mesh
    {
    public:
        explicit mesh(Alloc& al = Alloc()) noexcept
            : _meta{ 0, 0, 0, 0, mesh_topology::undefined }
            , _verts{ nullptr }
            , _index{ nullptr }
        {
        }

        explicit mesh(
            const std::vector<std::byte>& verts,
            const std::vector<std::byte>& index,
            Alloc&                        al = Alloc()) noexcept
            : _meta{}
            , _alloc{ al }
        {
            _verts = std::allocator_traits<Alloc>::allocate(std::size(verts));
            std::copy(std::cbegin(verts), std::cend(verts), _verts);

            _index = std::allocator_traits<Alloc>::allocate(std::size(index));
            std::copy(std::cbegin(index), std::cend(index), _index);
        }

        template <typename Allocator>
        explicit mesh(memory_stream<Allocator>& ms, Alloc& al = Alloc()) noexcept
        {
            // TODO: end impl
        }

        mesh(const mesh&) = delete;
        mesh& operator=(const mesh&) = delete;

        constexpr mesh(mesh&& other) noexcept
            : _meta(std::move(other._meta))
            , _alloc(std::move(other._alloc))
            , _verts(std::move(other._verts))
            , _index(std::move(other._index))
        {
        }

        constexpr mesh& operator=(mesh&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                std::swap(_meta, other._meta);
                std::swap(_alloc, other._alloc);
                std::swap(_verts, other._verts);
                std::swap(_index, other._index);
            }
            return *this;
        }

        [[nodiscard]] explicit operator mesh_view() const noexcept
        {
            return mesh_view{ _verts, _meta.vertex_count(), _index, _meta.index_count() };
        }

        // Returns the number of vertices stored in the vertex buffer.
        [[nodiscard]] DRAKO_FORCE_INLINE constexpr size_t vertex_count() const noexcept;

        // Returns the number of indexes stored in the index buffer.
        [[nodiscard]] DRAKO_FORCE_INLINE constexpr size_t index_count() const noexcept;

        // Returns a pointer to the vertex buffer.
        [[nodiscard]] DRAKO_FORCE_INLINE const std::byte*
        vertex_data() const noexcept { return _verts; }

        // Returns a pointer to the index buffer.
        [[nodiscard]] DRAKO_FORCE_INLINE const std::byte*
        index_data() const noexcept { return _index; }

        friend std::istream& operator>>(std::istream&, mesh&);
        friend std::ostream& operator<<(std::ostream&, const mesh&);

    private:
        mesh_desc_t      _meta;
        Alloc            _alloc;
        const std::byte* _verts;
        const std::byte* _index;
    };

    template <typename Allocator>
    std::istream& operator>>(std::istream& is, mesh<Allocator>& m);

    template <typename Allocator>
    std::ostream& operator<<(std::ostream& os, const mesh<Allocator>& m)
    {
        os.write(reinterpret_cast<const char*>(&m._meta), sizeof(decltype(m._meta)));
        os.write(m._verts, m._meta.vertex_bytes() * m._meta.vertex_count());
        os.write(m._index, m._meta.index_bytes() * m._meta.index_count());
        return os;
    }

} // namespace drako

#endif // !DRAKO_MESH_TYPES_HPP_