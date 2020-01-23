#pragma once
#ifndef DRAKO_GUI_HBOX_HPP
#define DRAKO_GUI_HBOX_HPP

#include "drako/gui/dk_container.hpp"
#include "drako/gui/dk_layout.hpp"
#include "drako/math/dk_aarect.hpp"

namespace drako
{


    // Widget that arranges its slots in an horizontal layout.
    //
    class HBox
    {
    public:

        explicit HBox(size_t capacity) noexcept;
        ~HBox() noexcept;
        
    private:



        static void hbox_update_geometry(AARect space) noexcept;
    };

    HBox::HBox(size_t capacity) noexcept
    {

    }

} // namespace drako

#endif // !DRAKO_GUI_HBOX_HPP
