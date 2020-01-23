#ifndef DRAKO_ROOT_HPP_
#define DRAKO_ROOT_HPP_

#include <vector>

#include "drako_gui_events.hpp"

namespace drako::gui
{
    using ID = std::uint32_t;

    /// <summary>Root of the gui hierarchy of the application.</summary>
    class Root
    {
    public:

        constexpr explicit Root();

        void dispatch_gui_event(MouseEventCode eventType, void* data);

    private:

        std::vector<ID> m_hierarchy;
    };
}

#endif // _DRAKO_ROOT_HPP_

