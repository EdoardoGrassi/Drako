#ifndef DRAKO_GUI_BUTTON_HPP
#define DRAKO_GUI_BUTTON_HPP

#include <drako_ecs.hpp>

namespace drako::gui
{
    class Button final
    {
    public:

        constexpr explicit Button();

    private:

        ecs::EntityID _guid;
    };
}

#endif // !DRAKO_GUI_BUTTON_HPP