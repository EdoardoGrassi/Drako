#pragma once
#ifndef DRAKO_MESH_TYPES_HPP
#define DRAKO_MESH_TYPES_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/system/memory_stream.hpp"

#include <cstddef>
#include <istream>
#include <type_traits>
#include <vector>

namespace drako
{
    // Uniquely identifies a mesh resource.
    enum class mesh_guid : std::uint32_t;

    DRAKO_NODISCARD DRAKO_FORCE_INLINE std::string to_string(mesh_guid id)
    {
        return std::to_string(static_cast<std::underlying_type_t<mesh_guid>>(id));
    }

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
        std::uint32_t vertex_count;

        // Number of indices in the mesh index buffer.
        std::uint32_t index_count;

        std::uint8_t vertex_size_bytes;

        std::uint8_t index_size_bytes;

        // Topology of the mesh.
        mesh_topology topology;

        // Byte size of a single index element.
        DRAKO_NODISCARD size_t index_bytes() const noexcept { return index_size_bytes; }

        // Elements in the index container.
        DRAKO_NODISCARD size_t index_count() const noexcept { return index_count; }

        // Byte size of a single vertex element.
        DRAKO_NODISCARD size_t vertex_bytes() const noexcept { return vertex_size_bytes; }

        // Elements in the vertex container.
        DRAKO_NODISCARD size_t vertex_count() const noexcept { return vertex_count; }
    };
    static_assert(std::is_trivially_copy_constructible_v<mesh_desc_t>,
        "Required for direct serialization with memcpy");

    DRAKO_NODISCARD DRAKO_FORCE_INLINE size_t
    vertex_buffer_size_bytes(const mesh_desc_t& desc) noexcept
    {
        return desc.vertex_count() * desc.vertex_bytes();
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE size_t
    index_buffer_size_bytes(const mesh_desc_t& desc) noexcept
    {
        return desc.index_count() * desc.index_bytes();
    }


    // Mesh asset.
    template <typename Alloc = std::allocator<std::byte>>
    class mesh
    {
    public:
        explicit mesh(Alloc& al = Alloc()) noexcept
            : _meta{ 0, 0, 0, 0, mesh_topology::undefined }
            , _verts{ nullptr }
            , _trias{ nullptr }
        {
        }

        /*
        template <typename V, typename I>
        explicit mesh(const std::vector<V>& vertices, const std::vector<I>& indexes, mesh_topology t, Alloc& al = Alloc())
            : _desc{ vertices.size(), sizeof(V), indexes.size(), sizeof(I), t }
            , _vertices(vertices, al)
            , _indexes(indexes, al)
        {
        }
        */

        template <typename Allocator>
        explicit mesh(memory_stream<Allocator>& ms, Alloc& al = Alloc()) noexcept
        {
            // TODO: end impl
        }

        mesh(const mesh&) = delete;
        mesh& operator=(const mesh&) = delete;

        constexpr mesh(mesh&& rhs) noexcept
            : _meta(std::move(rhs._meta))
            , _verts(std::move(rhs._verts))
            , _trias(std::move(rhs._trias))
        {
        }

        constexpr mesh& operator=(mesh&& rhs) noexcept
        {
            if (this != std::addressof(rhs))
            {
                std::swap(_meta, rhs._meta);
                std::swap(_verts, rhs._verts);
                std::swap(_trias, rhs._trias);
            }
            return *this;
        }

        DRAKO_NODISCARD explicit operator mesh_view() const noexcept
        {
            return mesh_view(_verts, size(_vertices), _trias, size(_indexes));
        }

        // Returns the number of vertices stored in the vertex buffer.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t vertex_count() const noexcept;

        // Returns the number of indexes stored in the index buffer.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t index_count() const noexcept;

        // Returns a pointer to the vertex buffer.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE const std::byte*
        vertex_data() const noexcept { return _verts; }

        // Returns a pointer to the index buffer.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE const std::byte*
        index_data() const noexcept { return _trias; }

    private:
        mesh_desc_t      _meta;
        const std::byte* _verts;
        const std::byte* _trias;
    };


    class mesh_view
    {
    public:
        explicit constexpr mesh_view() noexcept
            : _v_data{ nullptr }
            , _i_data{ nullptr }
            , _v_size{ 0 }
            , _i_size{ 0 }
        {
        }

        explicit constexpr mesh_view(const std::byte* vertex_data, size_t vertex_bytes,
            const std::byte* index_data, size_t index_bytes) noexcept
            : _v_data{ vertex_data }
            , _i_data{ index_data }
            , _v_size{ vertex_bytes }
            , _i_size{ index_bytes }
        {
            DRAKO_ASSERT(vertex_data != nullptr);
            DRAKO_ASSERT(index_data != nullptr);
            DRAKO_ASSERT(vertex_bytes != 0);
            DRAKO_ASSERT(index_bytes != 0);
        }

        mesh_view(const mesh_view&) noexcept = default;
        mesh_view& operator=(const mesh_view&) noexcept = default;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr const std::byte*
        vertex_data() const noexcept { return _v_data; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr const std::byte*
        index_data() const noexcept { return _i_data; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t
        vertex_data_size() const noexcept { return _v_size; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t
        index_data_size() const noexcept { return _i_size; }

    private:
        const std::byte* _v_data;
        const std::byte* _i_data;
        const size_t     _v_size;
        const size_t     _i_size;
    };

} // namespace drako

#endif // !DRAKO_MESH_TYPES_HPP