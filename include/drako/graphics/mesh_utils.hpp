#pragma once
#ifndef DRAKO_MESH_UTILS_HPP_
#define DRAKO_MESH_UTILS_HPP_

#include "drako/file_formats/wavefront_obj.hpp"
#include "drako/graphics/mesh_types.hpp"

#include <iterator>

namespace drako
{
    template <typename VertexAttributeIter, typename Allocator>
    mesh<Allocator> from_obj_source(
        VertexAttributeIter              first,
        VertexAttributeIter              last,
        const file_formats::obj::object& source)
    {
        using _vadesc = vertex_attribute_descriptor;
        using _traits = std::iterator_traits<VertexAttributeIter>;

        static_assert(std::is_same_v<_traits::value_type, _vadesc>,
            "Required iterator with value type " DRAKO_STRINGIZE(vertex_attribute_descriptor));
        static_assert(std::is_base_of_v<std::input_iterator_tag, _traits::iterator_category>,
            "Required iterator with input category or stronger");

        // TODO: end impl
    }

    template <typename Allocator, size_t M, size_t N>
    mesh<Allocator> from_obj_source(
        const std::array<vertex_attribute_descriptor, M>& verts_layout,
        const vertex_attribute_descriptor&                index_layout,
        const file_formats::obj::object&                  source,
        Allocator&                                        al)
    {
        using _al        = Allocator;
        using _al_traits = std::allocator_traits<_al>;

        // compute attribute verts_stride
        size_t verts_stride;
        for (const auto attribute& : verts_layout)
        {
            verts_stride += meta::_sizeof(attribute.format) * attribute.dimension;
        }
        const size_t verts_count = std::size(source.vertex_points);

        const size_t index_stride = meta::_sizeof(index_layout.format);
        const size_t index_count  = std::size(source.faces);

        // allocate buffers
        mesh<_al> result{};

        const auto verts = _al_traits::allocate(al, verts_stride * verts_count);

        const auto index = _al_traits::allocate(al, index_stride * index_count);
        // TODO: end impl
    }

} // namespace drako

#endif // !DRAKO_MESH_UTILS_HPP_