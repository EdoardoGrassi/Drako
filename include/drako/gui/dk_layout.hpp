#ifndef DRAKO_GUI_LAYOUT_HPP
#define DRAKO_GUI_LAYOUT_HPP

#include <cstdint>

#include "math/dk_aarect.hpp"

namespace drako
{
    // Common unit of reference used by the GUI system.
    using GuiUnit = uint32_t;


    // ENUM
    // Defines the type of layout used by a GUI element.
    enum class LayoutPolicy
    {
        LAYOUT_NONE,                            // No layout specified.
        LAYOUT_DIMENSIONS_FIXED,                // Dimensions specified as pixels.
        LAYOUT_DIMENSIONS_SCREEN_RELATIVE,      // Dimensions specified as proportional to screen size.
        LAYOUT_DIMENSIONS_WINDOW_RELATIVE,      // Dimensions specified as proportional to window size.
        LAYOUT_DIMENSIONS_CONTAINER_RELATIVE    // Dimensions specified as proportional to container size.
    };


    // Layout settings of a widget.
    //
    struct LayoutDescriptor
    {
        GuiUnit w_min;
        GuiUnit h_min;

        GuiUnit w_max;
        GuiUnit h_max;

        LayoutPolicy w_layout;
        LayoutPolicy h_layout;
    };

    DRAKO_NODISCARD AARect
        update_geometry(const AARect r, const LayoutDescriptor& l) noexcept
    {

    }
    

    // Defines where the content of a widget is anchored.
    //
    enum class AnchorPolicy
    {
        ANCHOR_CENTER,          // Anchors to the center of the widget.
        ANCHOR_TOP_LEFT,        // Anchors to the top left corner of the widget.
        ANCHOR_TOP,             // Anchors to the top edge of the widget.
        ANCHOR_TOP_RIGHT,       // Anchors to the top right corner of the widget.
        ANCHOR_RIGHT,           // Anchors to the right edge of the widget.
        ANCHOR_BOTTOM_RIGHT,    // Anchors to the bottom right corner of the widget.
        ANCHOR_BOTTOM,          // Anchors to the bottom edge of the widget.
        ANCHOR_BOTTOM_LEFT,     // Anchors to the bottom left corner of the widget.
        ANCHOR_LEFT             // Anchors to the left edge of the widget.
    };

} // namespace drako::gui

#endif // !DRAKO_GUI_LAYOUT_HPP
