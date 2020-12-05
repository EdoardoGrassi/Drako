#pragma once
#ifndef DRAKO_WIDGET_HPP
#define DRAKO_WIDGET_HPP

#include <cstdlib>
#include <cstdint>

#include "drako/core/compiler.hpp"

namespace drako
{
    using WidgetGUID = const uint32_t;

    DRAKO_NODISCARD DRAKO_FORCE_INLINE WidgetGUID make_widget_guid() noexcept
    {
        static uint32_t next_available = 1;

        if (DRAKO_UNLIKELY(next_available == 0))
            std::exit(EXIT_FAILURE);
        return next_available++;
    }
}

#endif // !DRAKO_WIDGET_HPP