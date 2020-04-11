#pragma once
#ifndef DRAKO_THREAD_HPP
#define DRAKO_THREAD_HPP

#include <cstdlib>

#include "drako/concurrency/thread_context.hpp"
#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/system/system_info.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <processthreadsapi.h>
#endif

namespace drako::sys
{
    // OS level native thread.
    class native_thread final
    {
    public:
        using id_type       = size_t;
        using priority_type = size_t;
        using routine_type  = unsigned long(DRAKO_API_STDCALL)(void* args);


        explicit native_thread(routine_type routine, size_t stack_size) noexcept
        {
#if defined(DRAKO_PLT_WIN32)
            _handle = ::CreateThread(NULL, // [In] security attributes
                stack_size,
                routine,
                nullptr,
                NULL,
                &_id); // [Out] created thread id
            if (_handle == NULL)
            {
                std::exit(EXIT_FAILURE);
            }
#else
#error Platform not supported
#endif
        }

        ~native_thread() noexcept
        {
#if defined(DRAKO_PLT_WIN32)

            if (_handle != INVALID_HANDLE_VALUE)
                ::CloseHandle(_handle);

#else
#error Platform not supported
#endif
        }

        native_thread(const native_thread&) = delete;
        native_thread& operator=(const native_thread&) = delete;

        native_thread(native_thread&& other) noexcept
        {
#if defined(DRAKO_PLT_WIN32)

            _handle       = other._handle;
            _id           = other._id;
            other._handle = INVALID_HANDLE_VALUE;

#else
#error Platform not supported
#endif
        }
        native_thread& operator=(native_thread&& other) noexcept
        {
#if defined(DRAKO_PLT_WIN32)

            _handle       = other._handle;
            _id           = other._id;
            other._handle = INVALID_HANDLE_VALUE;

#else
#error Platform not supported
#endif
            return *this;
        }

        void run(const thread_context&) noexcept;

        DRAKO_NODISCARD priority_type get_priority() const noexcept;

        void set_priority(const priority_type p) noexcept;

        DRAKO_NODISCARD native_cpu_core get_core_affinity() noexcept;

        bool set_core_affinity(native_cpu_core core) noexcept
        {
#if defined(DRAKO_PLT_WIN32)

            // PROCESSOR_NUMBER p;
            // memset(&p, 0, sizeof(p));
            // p.Number =
            return ::SetThreadIdealProcessorEx(_handle, &(core.cpu_number), NULL);

#else
#error Platform not supported
#endif
        }

    private:
#if defined(DRAKO_PLT_WIN32)

        HANDLE _handle;
        DWORD  _id;

#else
#error Platform not supported
#endif
    };

} // namespace drako::sys

#endif // !DRAKO_THREAD_HPP