/// @file
/// @brief   Provides macro definitions related to compiler-specific functionalities.
/// @author  Grassi Edoardo

#pragma once
#ifndef DRAKO_COMPILER_MACROS_HPP
#define DRAKO_COMPILER_MACROS_HPP

#include "drako/core/preprocessor/utility_macros.hpp"

/*vvv COMPILER DETECTION vvv*/

#ifdef _MSC_VER
// Defined for Microsoft MSVC compiler.
#define DRAKO_CC_MSVC
#endif


#ifdef __GNUC__
// Defined for GNU Gcc compiler.
#define DRAKO_CC_GCC
#endif


#ifdef __clang__
// Defined for CLang compiler.
#define DRAKO_CC_CLANG
#endif


/*vvv CALLING CONVENTIONS vvv*/

#if defined(DRAKO_CC_MSVC)
// MACRO: standard C calling convention.
#define DRAKO_API_CDECL __cdecl
#elif defined(DRAKO_CC_GCC)
#define DRAKO_API_CDECL __attribute__((cdecl))
#else
#define DRAKO_API_CDECL
#endif


#if defined(DRAKO_CC_MSVC)
// MACRO: standard calling convention for Win32 API.
#define DRAKO_API_STDCALL __stdcall
#elif defined(DRAKO_CC_GCC)
#define DRAKO_API_STDCALL __attribute__((stdcall))
#else
#define DRAKO_API_STDCALL
#endif


// MACRO: Microsoft specific calling convention.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_API_FASTCALL __fastcall
#elif defined(DRAKO_CC_GCC)
#define DRAKO_API_FASTCALL [[gnu::fastcall]]
#else
#define DRAKO_API_FASTCALL
#endif


// MACRO: Microsoft specific calling conventions.
// Extends support for vector-like arguments to SIMD registers and homogeneus vector aggregate (HVA) values.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_API_VECTORCALL __vectorcall
#elif defined(DRAKO_CC_GCC)
#define DRAKO_API_VECTORCALL [[gnu::vectorcall]]
#elif defined(DRAKO_CC_CLANG)
#define DRAKO_API_VECTORCALL [[clang::vectorcall]]
#else
#define DRAKO_API_VECTORCALL
#endif


/*vvv COMPILATION vvv*/


// MACRO: emits a warning with formatted test.
#define DRAKO_WARNING(desc) message(__FILE__ "(" DRAKO_STRINGIZE(__LINE__) ") : warning: " DRAKO_STRINGIZE(desc))

#if defined(DRAKO_CC_MSVC)
#define DRAKO_CC_VERSION "MSC v." DRAKO_STRINGIZE(_MSC_VER)
#else
#pragma warning "Compiler version macro isn't defined for current compiler"
#define DRAKO_CC_VERSION "[Unknown compiler version]"
#endif


/*vvv COMPILER SPECIFIC ATTRIBUTES vvv*/

// MACRO: attribute used as placeholder when the compiler doesn't support a particular functionality.
#define DRKAPI_ATTRIBUTE_PLACEHOLDER [[drako::attribute_not_supported]]

// MACRO: variable/parameter attribute.
// Declares that a variable is not aliased.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_HPP_RESTRICT __restrict
#elif defined(DRAKO_CC_GCC)
#define DRAKO_HPP_RESTRICT __restrict__
#else
#define DRAKO_HPP_RESTRICT DRKAPI_ATTRIBUTE_PLACEHOLDER
#endif


// MACRO: function attribute.
// Declares that the function acts like malloc and returns pointers to memory that isn't aliased.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_ALLOCATOR __declspec(allocator)
#elif defined(DRAKO_CC_GCC)
#define DRAKO_ALLOCATOR __attribute__((malloc))
#else
#define DRAKO_ALLOCATOR DRKAPI_ATTRIBUTE_PLACEHOLDER
#endif


// MACRO: function attribute.
// Forces the compiler to bypass cost analisys befor attempting to inline the function.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_FORCE_INLINE __forceinline
#else
#define DRAKO_FORCE_INLINE inline
#endif


#if (DRAKO_CC_GCC)

// MACRO
// Hints the compiler that a boolean expression is likely to evaluate to true.
#define DRAKO_LIKELY(condition) __builtin_expect(!!(condition), true)

// MACRO
// Hints the compiler that a boolean expression is likely to evaluate to false.
#define DRAKO_UNLIKELY(condition) __builtin_expect(!!(condition), false)

#else
#define DRAKO_LIKELY(condition)   condition
#define DRAKO_UNLIKELY(condition) condition
#endif


// MACRO: function attribute.
// Prompts the compiler to produce a warning if the return value of a function is discarded.
#define DRAKO_NODISCARD [[nodiscard]]

// MACRO: function attribute.
// Prompts the compiler to emit a warning if the function is used.
#define DRK_DEPRECATED [[deprecated]]


// MACRO: function attribute.
// Declares that the function does not interfere with the global memory state except through the pointers in its parameter list.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_HPP_NOALIAS __declspec(noalias)
#elif defined(DRAKO_CC_CLANG)
#define DRAKO_HPP_NOALIAS __declspec(noalias)
#else
#define DRAKO_HPP_NOALIAS DRKAPI_ATTRIBUTE_PLACEHOLDER
#endif


// MACRO: function attribute.
// Declares that the function:
// - returns a value based only on its input parameters, compile-time constants and global memory state;
// - does not have any side effects other than its return value;
// - does not modify memory state, neither through pointers or global variables.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_HPP_PURE
#elif defined(DRAKO_CC_GCC)
#define DRAKO_HPP_PURE __attribute__((pure))
#else
#define DRAKO_HPP_PURE DRKAPI_ATTRIBUTE_PLACEHOLDER
#endif


// Function attribute.
// Declares that the function:
// - returns a value based only on its input parameters and compile-time constants;
// - does not have any side effects other than its return value;
// - does not read or modify memory state, neither through pointers or global variables.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_HPP_PURE_LOCAL DRKAPI_ATTRIBUTE_PLACEHOLDER
#elif defined(DRAKO_CC_GCC)
#define DRAKO_HPP_PURE_LOCAL __attribute__((const))
#else
#define DRAKO_HPP_PURE_LOCAL DRKAPI_ATTRIBUTE_PLACEHOLDER
#endif



#endif // !DRAKO_COMPILER_MACROS_HPP