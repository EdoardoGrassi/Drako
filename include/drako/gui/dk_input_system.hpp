#ifndef DRAKO_GUI_INPUT_SYSTEM_HPP
#define DRAKO_GUI_INPUT_SYSTEM_HPP

#include <cstdint>
#include <vector>

#include "core/utility/dk_delegate.hpp"
#include "dod/dk_data_table.hpp"
#include "gui/dk_widget_transform.hpp"
#include "math/dk_vector2.hpp"

namespace drako::gui
{
    using WidgetGUID = uint32_t;

    struct MouseEvent
    {

    };

    // CLASS
    // Dispatcher of events related to mouse inputs.
    //
    class MouseEventDispatcher final
    {
    public:

        explicit MouseEventDispatcher() noexcept;
        ~MouseEventDispatcher() noexcept = default;

        void insert_event_handler() noexcept;

        void remove_event_handler() noexcept;

    private:

        // dod::DataTable<WidgetGUID, WidgetTransform> const& _transforms;
        // dod::DataTable<WidgetGUID, SinglecastDelegate<void, MouseEvent>> _lclick;
        // dod::DataTable<WidgetGUID, SinglecastDelegate<void, MouseEvent>> _rclick;

        std::vector<WidgetGUID>                             _ids;
        std::vector<WidgetTransform>                        _transforms;
        std::vector<SinglecastDelegate<void, MouseEvent>>   _callbacks;

        // mouse wheel events etc...

        void on_left_click() noexcept
        {

        }

        void on_right_click() noexcept;
    };


    // CLASS
    // Dispatcher of events related to keyboard inputs.
    //
    class KeyboardEventDispatcher final
    {

    };
}

#endif // !DRAKO_GUI_INPUT_SYSTEM_HPP
