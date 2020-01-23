#ifndef DRAKO_GUI_BASEBLOCK_HPP_
#define DRAKO_GUI_BASEBLOCK_HPP_

#include "drako_gui_layout.hpp"

namespace drako::gui
{
    /// <summary>Base class of every widget that can be used to build the GUI.</summary>
    class BaseBlock
    {
    public:
        virtual ~BaseBlock() = 0;

        virtual void on_repaint() = 0;
        virtual void on_rebuild(const DkRect buildArea) = 0;
        virtual void handle_gui_event() = 0;
        virtual LayoutRect get_desired_size() = 0;
    };
}

#endif // !DRAKO_GUI_BASEBLOCK_HPP_

