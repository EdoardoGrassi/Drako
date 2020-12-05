#pragma once
#include "drako/system/system_info.hpp"

#include "drako/core/compiler.hpp"
#include "drako/core/platform.hpp"
#include "drako/devel/assertion.hpp"

#include <cstddef>

// #include <WinBase.h>
// #include <sysinfoapi.h>
#include <Windows.h>

#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

namespace drako::sys
{
    [[nodiscard]] native_cpu_core current_process_cpu() noexcept
    {
        PROCESSOR_NUMBER proc_number = {};
        ::GetCurrentProcessorNumberEx(&proc_number);
        return native_cpu_core{ proc_number };
    }


    [[nodiscard]] std::uint32_t cpu_logical_core_count() noexcept
    {
#if defined(DRAKO_PLT_WIN32)

        SYSTEM_INFO sys_info = {};
        ::GetSystemInfo(&sys_info);
        DRAKO_ASSERT(sys_info.dwNumberOfProcessors > 0);
        return sys_info.dwNumberOfProcessors;

#elif defined(DRAKO_PLT_LINUX)

        return get_nprocs();

#else
#error Platform not supported
#endif
    }

    [[nodiscard]] std::int64_t cpu_counter_value() noexcept
    {
#if defined(DRAKO_PLT_WIN32)

        LARGE_INTEGER counter = {};
        // doesn't produce errors on Windows XP or later
        ::QueryPerformanceCounter(&counter);

        return static_cast<std::int64_t>(counter.QuadPart);

#else
#error Platform not supported
#endif
    }

    [[nodiscard]] std::int64_t cpu_counter_frequency() noexcept
    {
#if defined(DRAKO_PLT_WIN32)

        LARGE_INTEGER frequency = {};
        // doesn't produce errors on Windows XP or later
        ::QueryPerformanceFrequency(&frequency);

        return static_cast<int64_t>(frequency.QuadPart);

#endif
    }

    [[nodiscard]] native_numa_node cpu_numa_node(native_cpu_core core) noexcept
    {
#if defined(DRAKO_PLT_WIN32)

        USHORT numa_node = {};
        if (::GetNumaProcessorNodeEx(&core.cpu_number, &numa_node) == 0)
        {
            // error
            std::exit(EXIT_FAILURE);
        }
        return native_numa_node{ numa_node };

#else
#error Platform not supported
#endif
    }
} // namespace drako::sys