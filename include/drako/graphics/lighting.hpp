#pragma once
#ifndef DRAKO_LIGHTING_HPP
#define DRAKO_LIGHTING_HPP

#include "drako/math/vector3.hpp"

namespace drako::gpx
{
    struct directed_light_src
    {
        vec3 direction;
        vec3 color;
    };

    struct point_light_src
    {
        vec3 position;
        vec3 color;
    };

} // namespace drako::gpx

#endif // !DRAKO_LIGHTING_HPP