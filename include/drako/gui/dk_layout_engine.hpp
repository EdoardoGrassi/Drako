#pragma once
#ifndef DRAKO_GUI_LAYOUT_SYSTEM_HPP
#define DRAKO_GUI_LAYOUT_SYSTEM_HPP

#include <vector>

#include "drako/gui/dk_container.hpp"
#include "drako/gui/dk_layout.hpp"
#include "drako/gui/dk_widget.hpp"
#include "drako/math/dk_aarect.hpp"

namespace drako
{
    class LayoutEngine
    {
    public:

        explicit LayoutEngine() noexcept;

        void update_layout_geometry(AARect user_area) noexcept;

    private:

        std::vector<WidgetGUID>         _guids;
        std::vector<LayoutManagerComponent>      _containers;
        std::vector<LayoutDescriptor>   _leafs;
        const std::vector<AARect>&      _transforms;
    };


    void LayoutEngine::update_layout_geometry(const AARect user_area) noexcept
    {
        // Gather all containers that may be modified

        // Foreach container, update children transforms
    }


    // Container that manages the layout of his children widget.
    //
    class LayoutManagerComponent
    {
        using layout_function = void(*)(AARect) noexcept;

    public:

        void update_geometry(AARect space) noexcept;

    private:

        std::vector<WidgetGUID>         _guids;
        std::vector<LayoutDescriptor>   _layout;
        std::vector<AARect&>            _transforms;
        layout_function                 _layouter;
    };

} // namespace drako

#endif // !DRAKO_GUI_LAYOUT_SYSTEM_HPP
