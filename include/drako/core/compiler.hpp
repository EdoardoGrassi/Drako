/// @file
/// @brief   Provides macro definitions related to compiler-specific functionalities.
/// @author  Grassi Edoardo

#pragma once
#ifndef DRAKO_COMPILER_HPP
#define DRAKO_COMPILER_HPP

#include "drako/core/preprocessor/utility_macros.hpp"

/*vvv COMPILER DETECTION vvv*/

#ifdef _MSC_VER
// Defined for Microsoft MSVC compiler.
#define DRAKO_CC_MSVC
#define _drako_compiler_msvc
#endif


#ifdef __GNUC__
// Defined for GNU Gcc compiler.
#define DRAKO_CC_GCC
#define _drako_compiler_gcc
#endif


#ifdef __clang__
// Defined for CLang compiler.
#define DRAKO_CC_CLANG
#define _drako_compiler_clang
#endif


#if defined(_drako_compiler_msvc)
#define _drako_flexible_array_member // MSVC extension uses syntax 'int array[]'
#else
#error Compiler extension 'flexible array member' is not available.
#endif


/*vvv CALLING CONVENTIONS vvv*/

/// @brief Standard C calling convention.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_API_CDECL __cdecl

#elif defined(DRAKO_CC_GCC)
#define DRAKO_API_CDECL __attribute__((cdecl))

#else
#pragma message("Calling convention CDECL not supported with current compiler.")
#define DRAKO_API_CDECL
#endif


/// @brief Standard calling convention for Win32 API.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_API_STDCALL __stdcall

#elif defined(DRAKO_CC_GCC)
#define DRAKO_API_STDCALL __attribute__((stdcall))

#else
#pragma message("Calling convention STDCALL not supported with current compiler.")
#define DRAKO_API_STDCALL
#endif


/// @brief Microsoft specific calling convention.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_API_FASTCALL __fastcall

#elif defined(DRAKO_CC_GCC)
#define DRAKO_API_FASTCALL [[gnu::fastcall]]

#else
#pragma message("Calling convention FASTCALL not supported with current compiler.")
#define DRAKO_API_FASTCALL
#endif


/// @brief Microsoft specific calling conventions.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_API_VECTORCALL __vectorcall

#elif defined(DRAKO_CC_GCC)
#define DRAKO_API_VECTORCALL [[gnu::vectorcall]]

#elif defined(DRAKO_CC_CLANG)
#define DRAKO_API_VECTORCALL [[clang::vectorcall]]

#else
#pragma message("Calling convention VECTORCALL not supported with current compiler.")
#define DRAKO_API_VECTORCALL
#endif


/*vvv COMPILATION vvv*/


// MACRO: emits a warning with formatted test.
#define DRAKO_WARNING(desc) message(__FILE__ "(" DRAKO_STRINGIZE(__LINE__) ") : warning: " DRAKO_STRINGIZE(desc))


/// @brief Descriptive string of the current compiler version.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_CC_VERSION "MSC v." DRAKO_STRINGIZE(_MSC_VER)

#else
#pragma message("Cannot detect current compiler version.")
#define DRAKO_CC_VERSION "[Unknown compiler version]"
#endif


/*vvv COMPILER SPECIFIC ATTRIBUTES vvv*/


// MACRO: variable/parameter attribute.
// Declares that a variable is not aliased.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_RESTRICT __restrict

#elif defined(DRAKO_CC_GCC)
#define DRAKO_RESTRICT __restrict__

#else
#pragma message("Attribute DRAKO_RESTRICT not supported with current compiler.")
#define DRAKO_RESTRICT
#endif


// MACRO: function attribute.
// Declares that the function acts like malloc and returns pointers to memory that isn't aliased.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_ALLOCATOR __declspec(allocator)

#elif defined(DRAKO_CC_GCC)
#define DRAKO_ALLOCATOR __attribute__((malloc))

#else
#pragma message("Attribute DRAKO_ALLOCATOR not supported with current compiler.")
#define DRAKO_ALLOCATOR
#endif


// MACRO: function attribute.
// Forces the compiler to bypass cost analisys befor attempting to inline the function.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_FORCE_INLINE __forceinline

#else
#pragma message("Attribute DRAKO_FORCE_INLINE not supported with current compiler.")
#define DRAKO_FORCE_INLINE
#endif


// MACRO: function attribute.
// Declares that the function does not interfere with the global memory state
// except through the pointers in its parameter list.
#if defined(DRAKO_CC_MSVC)
#define DRAKO_NOALIAS __declspec(noalias)

//#elif defined(DRAKO_CC_CLANG)
//#define DRAKO_NOALIAS __declspec(noalias)

#else
#pragma message("Attribute DRAKO_NOALIAS not supported with current compiler.")
#define DRAKO_NOALIAS
#endif

/*

// MACRO: function attribute.
// Declares that the function:
// - returns a value based only on its input parameters, compile-time constants and global memory state;
// - does not have any side effects other than its return value;
// - does not modify memory state, neither through pointers or global variables.
#if defined(DRAKO_CC_MSVC)
#pragma message("Attribute DRAKO_PURE not supported with current compiler.")
#define DRAKO_PURE

#elif defined(DRAKO_CC_GCC)
#define DRAKO_PURE __attribute__((pure))

#else
#pragma message("Attribute DRAKO_PURE not supported with current compiler.")
#define DRAKO_HPP_PURE DRKAPI_ATTRIBUTE_PLACEHOLDER
#endif


// Function attribute.
// Declares that the function:
// - returns a value based only on its input parameters and compile-time constants;
// - does not have any side effects other than its return value;
// - does not read or modify memory state, neither through pointers or global variables.
#if defined(DRAKO_CC_MSVC)
#pragma message("Attribute DRAKO_PURE_LOCAL not supported with current compiler.")
#define DRAKO_PURE_LOCAL

#elif defined(DRAKO_CC_GCC)
#define DRAKO_PURE_LOCAL __attribute__((const))

#else
#pragma message("Attribute DRAKO_PURE_LOCAL not supported with current compiler.")
#define DRAKO_PURE_LOCAL
#endif

*/

#endif // !DRAKO_COMPILER_HPP