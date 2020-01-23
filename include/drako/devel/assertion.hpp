#pragma once
#ifndef DRAKO_ASSERTION_HPP
#define DRAKO_ASSERTION_HPP

//
// \brief   Functions and macro definitions that implement runtime assertions.
// \author  Grassi Edoardo
//

#include <iostream>

#include "drako/core/preprocessor/compiler_macros.hpp"

namespace drako
{
// Forces the cpu into the debugger
#if defined(_MSC_VER)
#define DRAKO_FORCE_DEBUG_BREAK() __debugbreak()
#else
#define DRAKO_FORCE_DEBUG_BREAK() asm { int 3 }
#endif

    DRAKO_FORCE_INLINE void debug_assert(bool condition)
    {
        if (DRAKO_LIKELY(condition)) {}
        else
        {
            DRAKO_FORCE_DEBUG_BREAK();
        }
    }

    DRAKO_FORCE_INLINE void debug_assert(bool condition, const char* error_message_ptr)
    {
        if (DRAKO_LIKELY(condition)) {}
        else
        {
            std::cerr << error_message_ptr << std::endl;
            std::cerr.flush();
            DRAKO_FORCE_DEBUG_BREAK();
        }
    }


    template <typename T>
    inline void assert_not_null(const T* ptr)
    {
        debug_assert(ptr != nullptr);
    }

    template <typename T>
    inline void assert_not_null(const T* ptr, const char* error_message_ptr)
    {
        debug_assert(ptr != nullptr, error_message_ptr);
    }


    inline void assert_not_null(const void* ptr)
    {
        debug_assert(ptr != nullptr);
    }

    inline void assert_not_null(const void* ptr, const char* error_message_ptr)
    {
        debug_assert(ptr != nullptr, error_message_ptr);
    }


    template <typename T>
    inline void assert_inside_inclusive_bounds(T value, T min, T max, const char* pMessage)
    {
        debug_assert((value >= min) && (value <= max), pMessage);
    }

    template <typename T>
    inline void assert_inside_exclusive_bounds(T value, T min, T max, const char* pMessage)
    {
        debug_assert((value > min) && (value < max), pMessage);
    }


#if !defined(DRAKO_API_ASSERTIONS)

// Triggers if the expression evaluates to false
#define DRAKO_ASSERT(...)                 \
    do                                    \
    {                                     \
        drako::debug_assert(__VA_ARGS__); \
    } while (false)

// Asserts that the pointer isn't null
#define DRAKO_ASSERT_NOT_NULL(...)           \
    do                                       \
    {                                        \
        drako::assert_not_null(__VA_ARGS__); \
    } while (false)


#define DRAKO_ASSERT_INSIDE_INCLUSIVE_BOUNDS(value, min, max, error_message_ptr)           \
    do                                                                                     \
    {                                                                                      \
        drako::assert_inside_inclusive_bounds((value), (min), (max), (error_message_ptr)); \
    } while (false)

#else // Strip debug code
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

} // namespace drako

#endif // !DRAKO_ASSERTIONS_HPP