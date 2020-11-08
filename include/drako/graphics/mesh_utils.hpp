#pragma once
#ifndef DRAKO_MESH_UTILS_HPP_
#define DRAKO_MESH_UTILS_HPP_

#include "drako/file_formats/wavefront/object.hpp"
#include "drako/graphics/mesh_types.hpp"

#include <iterator>

namespace drako
{
    template <typename VertexAttributeIter>
    [[nodiscard]] mesh from_obj_source(VertexAttributeIter first, VertexAttributeIter last,
        const file_formats::obj::object& source)
    {
        using _vadesc = vertex_attribute_descriptor;
        using _traits = std::iterator_traits<VertexAttributeIter>;

        static_assert(std::is_same_v<_traits::value_type, _vadesc>,
            "Required iterator with value type " DRAKO_STRINGIZE(vertex_attribute_descriptor));
        static_assert(std::is_base_of_v<std::input_iterator_tag, _traits::iterator_category>,
            "Required iterator with input category or stronger");

        // TODO: end impl
        throw std::runtime_error{ "Not implemented" };
    }

} // namespace drako

#endif // !DRAKO_MESH_UTILS_HPP_