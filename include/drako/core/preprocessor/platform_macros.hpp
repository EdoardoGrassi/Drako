/// @file
/// @brief      Provides symbols definitions related to the target compilation platform.
/// @author     Grassi Edoardo
/// @date       Last modified on 06/06/2019

#pragma once
#ifndef DRAKO_PLATFORM_MACROS_HPP
#define DRAKO_PLATFORM_MACROS_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"

#if defined(DRAKO_CC_MSVC)

// MACRO
// Defined for Win32/64 target.
//
#define DRAKO_PLT_WIN32 _WIN32

// TODO: add platform targets to cmake config

// MACRO
// Defined for OSX target.
//
// #define DRAKO_PLT_MACOS

// MACRO
// Defined for Linux target.
//
// #define DRAKO_PLT_LINUX

#endif // DRKAPI_CC_MSC


#if defined(DRAKO_CC_MSVC) && defined(_M_X86)
#define DRAKO_ARCH_X86
#endif

#if defined(DRAKO_CC_MSVC) && defined(_M_X64)
#define DRAKO_ARCH_X64
#endif


#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
// defined for Intel(c) target architectures
#define DRAKO_ARCH_INTEL
#endif


#endif // !DRAKO_PLATFORM_MACROS_HPP