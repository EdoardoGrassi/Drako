/// @file
/// @brief      Provides symbols definitions related to the target compilation platform.
/// @author     Grassi Edoardo
/// @date       Last modified on 06/06/2019

#pragma once
#ifndef DRAKO_PLATFORM_MACROS_HPP
#define DRAKO_PLATFORM_MACROS_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"

#if defined(DRAKO_CC_MSVC)

/// @brief Defined when Windows is the target platform.
#define _drako_platform_Win32 _WIN32

/// @brief [[deprecated]] Use '_drako_platform_Win32' instead.
#define DRAKO_PLT_WIN32 _drako_platform_Win32

// TODO: add platform targets to cmake config

// MACRO
// Defined for OSX target.
//
// #define DRAKO_PLT_MACOS
// #define _drako_platform_MacOS

// MACRO
// Defined for Linux target.
//
// #define DRAKO_PLT_LINUX
// #define _drako_platform_Linux

#endif // DRKAPI_CC_MSC


#if defined(DRAKO_CC_MSVC) && defined(_M_X86)
#define DRAKO_ARCH_X86
#endif

#if defined(DRAKO_CC_MSVC) && defined(_M_X64)

/// @brief Defined when Intel x64 is the target architecture.
#define _drako_arch_x64

/// @brief [[deprecated]] Use _drako_arch_x64 instead.
#define DRAKO_ARCH_X64

#endif


#if defined(_drako_arch_x86) || defined(_drako_arch_x64)

/// @brief Defined when Intel is the target architecture.
#define _drako_arch_intel

/// @brief [[deprecated]] Use _drako_arch_intel instead.
#define DRAKO_ARCH_INTEL

#endif


#endif // !DRAKO_PLATFORM_MACROS_HPP