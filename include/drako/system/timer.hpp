#pragma once
#ifndef DRAKO_SYSTEM_TIMER_HPP_
#define DRAKO_SYSTEM_TIMER_HPP_

#include "drako/core/platform.hpp"

#if defined(DRAKO_PLT_WIN32)
#include "drako/system/src/timers_win32.hpp"

namespace drako::sys
{
    using periodic_timer = _threadpool_periodic_timer;
} // namespace drako::sys

#else

#error Platform currently not supported
#endif

#endif // !DRAKO_SYSTEM_TIMER_HPP_