/// @file
/// @brief  Core header with Drako definitions.
/// @author Grassi Edoardo

#pragma once
#ifndef DRAKO_API_WIN_HPP
#define DRAKO_API_WIN_HPP

#include "drako/core/preprocessor/platform_macros.hpp"

#include <cstdint>

// Version number format: [ 10-bit MAJOR ] [ 10-bit MINOR ] [ 12-bit PATCH ]
#define DRAKO_MAKE_VERSION(major, minor, patch) (((major) << 22) | ((minor) << 12) | (patch))

// Extracts major number from api version number.
#define DRAKO_VERSION_MAJOR(version) ((version) >> 22)

// Extracts minor number from api version number.
#define DRAKO_VERSION_MINOR(version) (((version) >> 12) & 0x3ff)

// Extracts patch number from api version number.
#define DRAKO_VERSION_PATCH(version) ((version)&0xfff)

namespace drako
{

/* clang-format off */

// Drako version number.
#define DRAKO_API_VERSION DRAKO_MAKE_VERSION(${PROJECT_VERSION_MAJOR}, ${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_PATCH})

using api_version = std::uint32_t;

[[nodiscard]] DRAKO_FORCE_INLINE constexpr const api_version
build_api_version() noexcept
{
    //return DRAKO_MAKE_VERSION(${PROJECT_VERSION_MAJOR}, ${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_PATCH});
    return DRAKO_API_VERSION;
}

    /* clang-format on */

} // namespace drako

// Drako base namespace.
#define DRAKO_HPP_NAMESPACE drako


#endif // !DRAKO_API_WIN_HPP