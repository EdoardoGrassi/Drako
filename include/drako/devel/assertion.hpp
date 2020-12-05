#pragma once
#ifndef DRAKO_ASSERTION_HPP_
#define DRAKO_ASSERTION_HPP_

/// @file
/// @brief   Functions and macro definitions that implement runtime assertions.
/// @author  Grassi Edoardo

#include <iostream>

#include "drako/core/compiler.hpp"

namespace drako
{
    // Forces the cpu into the debugger
    [[no_return]] DRAKO_FORCE_INLINE void _raise_debug_break()
    {
#if defined(DRAKO_CC_MSVC)
        __debugbreak();
#else
        asm { int 3 }
#endif
    }

    DRAKO_FORCE_INLINE void debug_assert(bool condition)
    {
        if (!condition)
            [[unlikely]] _raise_debug_break();
    }

    DRAKO_FORCE_INLINE void debug_assert(bool condition, const char* error_message_ptr)
    {
        if (!condition)
        {
            [[unlikely]] std::cerr << error_message_ptr << std::endl;
            _raise_debug_break();
        }
    }

    template <typename T>
    inline void assert_not_null(const T* ptr)
    {
        debug_assert(ptr != nullptr);
    }

    template <typename T>
    inline void assert_not_null(const T* ptr, const char* msg)
    {
        debug_assert(ptr != nullptr, msg);
    }


    inline void assert_not_null(const void* ptr)
    {
        debug_assert(ptr != nullptr);
    }

    inline void assert_not_null(const void* ptr, const char* msg)
    {
        debug_assert(ptr != nullptr, msg);
    }


    template <typename T>
    inline void assert_inside_inclusive_bounds(T value, T min, T max, const char* msg)
    {
        debug_assert((value >= min) && (value <= max), msg);
    }

    template <typename T>
    inline void assert_inside_exclusive_bounds(T value, T min, T max, const char* msg)
    {
        debug_assert((value > min) && (value < max), msg);
    }

} // namespace drako


#if !defined(DRAKO_API_ASSERTIONS)

// Triggers if the expression evaluates to false
#define DRAKO_ASSERT(...)                   \
    do                                      \
    {                                       \
        ::drako::debug_assert(__VA_ARGS__); \
    } while (false)

// Asserts that the pointer isn't null
#define DRAKO_ASSERT_NOT_NULL(...)             \
    do                                         \
    {                                          \
        ::drako::assert_not_null(__VA_ARGS__); \
    } while (false)


#define DRAKO_ASSERT_INSIDE_INCLUSIVE_BOUNDS(value, min, max, error_message_ptr)             \
    do                                                                                       \
    {                                                                                        \
        ::drako::assert_inside_inclusive_bounds((value), (min), (max), (error_message_ptr)); \
    } while (false)

#else // ^^^ actual definitions (debug) ^^^/vvv empty definitions (release) vvv
#
#define DRAKO_ASSERT(...)
#define DRAKO_ASSERT_NOT_NULL(...)
#define DRAKO_ASSERT_INSIDE_BOUNDS(...)
#
#endif // !DRAKO_API_ASSERTIONS


// \brief   Declares that an assertion is tested as part of a function precodition invariant.
#define DRAKO_PRECON(condition) DRAKO_ASSERT(condition)

// \brief   Declares that an assertion is tested as part of a function postcondition invariant.
#define DRAKO_POSTCON(condition) DRAKO_ASSERT(condition)

#endif // !DRAKO_ASSERTIONS_HPP_