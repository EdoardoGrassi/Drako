#pragma once
#ifndef DRAKO_SYSTEM_TIMERS_WIN32_HPP_
#define DRAKO_SYSTEM_TIMERS_WIN32_HPP_

#include "drako/core/preprocessor/platform_macros.hpp"

#include <chrono>
#include <exception>
#include <limits>

#include <threadpoolapiset.h>

#if !defined(DRAKO_PLT_WIN32)
#error Wrong target platform - this header should be included only in Windows builds
#endif

namespace drako::sys
{
    // Timer object that invokes a routine after a countdown.
    class threadpool_timer
    {
    };

    // Timer object that invokes a routine periodically.
    class _threadpool_periodic_timer
    {
    public:
        using callback_type = void (*)();

        [[nodiscard]] static constexpr std::chrono::milliseconds min_period()
        {
            return std::chrono::milliseconds{ 1 };
        }

        [[nodiscard]] static constexpr std::chrono::milliseconds max_period()
        {
            return std::chrono::milliseconds{ std::numeric_limits<DWORD>::max() };
        }

        template <typename Rep, typename Period>
        explicit _threadpool_periodic_timer(std::chrono::duration<Rep, Period> interval, callback_type callback)
        {
            namespace _chr                        = std::chrono;
            const auto system_provided_threadpool = NULL;

            // store callback inside user data payload
            _timer = ::CreateThreadpoolTimer(_wrapper, callback, system_provided_threadpool);
            if (_timer == NULL)
                std::exit(EXIT_FAILURE);
            // throw std::system_error(::GetLastError());

            // TODO: add exception throwing version

            ULARGE_INTEGER temp       = {};
            temp.QuadPart             = -10'000'000; // * 100 nanoseconds (see Windows doc)
            FILETIME expiration       = {};
            expiration.dwLowDateTime  = temp.LowPart;
            expiration.dwHighDateTime = temp.HighPart;

            const auto system_interval  = _chr::duration_cast<_chr::milliseconds>(interval).count();
            const auto system_precision = system_interval / 10;
            ::SetThreadpoolTimer(_timer, &expiration, system_interval, system_precision);
        }

        _threadpool_periodic_timer(const _threadpool_periodic_timer& other) noexcept = delete;
        _threadpool_periodic_timer& operator=(const _threadpool_periodic_timer& other) noexcept = delete;

        _threadpool_periodic_timer(_threadpool_periodic_timer&& other) noexcept = delete;
        _threadpool_periodic_timer& operator=(_threadpool_periodic_timer&& other) noexcept = delete;

        ~_threadpool_periodic_timer() noexcept
        {
            const bool cancel_pending_callbacks = true;

            ::SetThreadpoolTimer(_timer, NULL, 0, 0);
            ::WaitForThreadpoolTimerCallbacks(_timer, cancel_pending_callbacks);
            ::CloseThreadpoolTimer(_timer);
        }

    private:
        PTP_TIMER _timer;

        static void _wrapper(PTP_CALLBACK_INSTANCE instance, PVOID ctx, PTP_TIMER timer) noexcept
        {
            // fetch and invoke callback from system provided payload
            std::invoke(static_cast<callback_type>(ctx));
        }
    };

} // namespace drako::sys

#endif // !DRAKO_SYSTEM_TIMERS_WIN32_HPP_