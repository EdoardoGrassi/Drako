#pragma once
#ifndef DRAKO_POLYLINE_HPP
#define DRAKO_POLYLINE_HPP

#include <vector>

#include "core/compiler_macros.hpp"
#include "math/dk_vector2.hpp"

namespace drako::math
{
    // CLASS
    // Sequence of segments in 2D space.
    class Polyline2D
    {
    public:

        // Returns the number of points in the polyline.
        constexpr size_t size() const noexcept { return _points.size(); }

    private:
        std::vector<Point2D> _points;
    };

} // namespace drako::math

#endif // !DRAKO_POLYLINE_HPP

