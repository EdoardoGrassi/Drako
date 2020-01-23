#ifndef DRAKO_GUI_CONTAINER_HPP
#define DRAKO_GUI_CONTAINER_HPP

#include <cstdlib>

#include "math/dk_aarect.hpp"

namespace drako::gui
{
    // Container that manages layout of nested widgets.
    struct LayoutContainer
    {
        using LayoutFunc = void(*)(const math::AARect);

        size_t child_index;

        size_t child_count;

        /// <summary>Function used for layout computation.</summary>
        LayoutFunc pLayoutFunction;
    };

} // namespace drako::gui

#endif // !DRAKO_GUI_CONTAINER_HPP

