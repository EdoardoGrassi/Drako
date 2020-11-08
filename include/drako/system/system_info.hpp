#pragma once
#ifndef DRAKO_SYSTEM_INFO_HPP
#define DRAKO_SYSTEM_INFO_HPP

#include <cstdint>

#include "drako/core/preprocessor/platform_macros.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
// #include <processthreadsapi.h>
#endif

namespace drako::sys
{
    // Platform specific descriptor of a logical processor.
    struct native_cpu_core
    {
#if defined(DRAKO_PLT_WIN32)

        constexpr explicit native_cpu_core(PROCESSOR_NUMBER desc_)
            : cpu_number(desc_)
        {
        }

        PROCESSOR_NUMBER cpu_number;

#else
#error Platform not supported
#endif
    };


    // Gets the CPU that is executing the current process.
    //
    [[nodiscard]] native_cpu_core current_process_cpu() noexcept;

    // Gets the number of processor cores currently available in the system.
    //
    [[nodiscard]] uint32_t cpu_logical_core_count() noexcept;

    // Gets the byte size of a single page of memory.
    //
    [[nodiscard]] uint32_t cpu_memory_page_size() noexcept;

    // Gets the current value of the CPU high-precision counter.
    //
    [[nodiscard]] int64_t cpu_counter_value() noexcept;

    // Gets the frequency of the CPU high-precision counter.
    //
    [[nodiscard]] int64_t cpu_counter_frequency() noexcept;


    // Platoform specific descriptor of a NUMA processor group.
    struct native_numa_node
    {
#if defined(DRAKO_PLT_WIN32)

        constexpr explicit native_numa_node(std::uint32_t id) noexcept
            : guid{ id }
        {
        }

        uint32_t guid;

#else
#error Platform not supported
#endif
    };

    // Returns the numa node of a logical processor.
    //
    [[nodiscard]] native_numa_node cpu_numa_node(native_cpu_core core) noexcept;
} // namespace drako::sys

#endif // !DRAKO_SYSTEM_INFO_HPP