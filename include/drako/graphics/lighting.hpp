#pragma once
#ifndef DRAKO_LIGHTING_HPP
#define DRAKO_LIGHTING_HPP

#include "drako/math/vector3.hpp"

namespace drako
{
    struct directed_light_source
    {
        vec3 direction;
        vec3 color;
    };

    struct point_light_source
    {
        vec3 position;
        vec3 color;
    };

} // namespace drako

#endif // !DRAKO_LIGHTING_HPP