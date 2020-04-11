#pragma once
#ifndef DRAKO_FIBER_HPP
#define DRAKO_FIBER_HPP

#include <cstdlib>

#include "drako/core/preprocessor/platform_macros.hpp"

namespace drako
{
    // CLASS
    // Execution context that can be run on a thread.
    class thread_context final
    {
    public:

        using fiber_routine = void(DRAKO_API_STDCALL)(void*) noexcept;

        explicit thread_context(fiber_routine routine) noexcept
        {
            #if defined(DRAKO_PLT_WIN32)
            {
                _address = ::CreateFiber(0/*default stack size*/, routine, nullptr);
                if (_address == nullptr)
                {
                    std::exit(EXIT_FAILURE);
                }
            }
            #else
            #error Platform currently not supported
            #endif
        }

        // @brief   Creates a fiber object.
        //
        explicit thread_context(fiber_routine start_routine, size_t stack_size) noexcept
        {
            #if defined(DRAKO_PLT_WIN32)
            {
                _address = ::CreateFiber(stack_size, start_routine, nullptr);
                if (_address == nullptr)
                {
                    std::exit(EXIT_FAILURE);
                }
            }
            #else
            #error Platform currently not supported
            #endif
        }

        ~thread_context() noexcept
        {
            #if defined(DRAKO_PLT_WIN32)

            if (_address != nullptr)
                ::DeleteFiber(_address);

            #else
            #error Platform currently not supported
            #endif
        }

        thread_context(const thread_context&) = delete;
        thread_context& operator=(const thread_context&) = delete;

        thread_context(thread_context&&) noexcept;
        thread_context& operator=(thread_context&&) noexcept;

        void switch_context(const thread_context& other) noexcept
        {
            #if defined(DRAKO_PLT_WIN32)

            ::SwitchToFiber(other._address);

            #else
            #error Platform currently not supported
            #endif
        }

    private:

        void* _address;
        void* _args;
    };
}

#endif // !DRAKO_FIBER_HPP

