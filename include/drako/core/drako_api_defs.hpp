/// @file
/// @brief  Core header with Drako definitions.
/// @author Grassi Edoardo

#pragma once
#ifndef DRAKO_API_DEFS_HPP
#define DRAKO_API_DEFS_HPP

#include "drako/core/preprocessor/platform_macros.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>


// Drako base namespace.
//#define DRAKO_HPP_NAMESPACE drako


namespace drako
{
    /// @brief Three-part version identification number.
    ///
    /// Version code is composed of major, minor and patch numbers (as in major.minor.patch).
    ///
    class major_minor_patch_version
    {
    public:
        constexpr explicit major_minor_patch_version(uint16_t major, uint16_t minor, uint32_t patch) noexcept;

        [[nodiscard]] constexpr bool operator==(const major_minor_patch_version&) noexcept;
        [[nodiscard]] constexpr bool operator!=(const major_minor_patch_version&) noexcept;
        [[nodiscard]] constexpr bool operator>(const major_minor_patch_version&) noexcept;
        [[nodiscard]] constexpr bool operator<(const major_minor_patch_version&) noexcept;
        [[nodiscard]] constexpr bool operator>=(const major_minor_patch_version&) noexcept;
        [[nodiscard]] constexpr bool operator<=(const major_minor_patch_version&) noexcept;

        /// @brief Major number.
        [[nodiscard]] constexpr uint16_t major() const noexcept;

        /// @brief Minor number.
        [[nodiscard]] constexpr uint16_t minor() const noexcept;

        /// @brief Patch number.
        [[nodiscard]] constexpr uint16_t patch() const noexcept;

        [[nodiscard]] static major_minor_patch_version from_string(const std::string&);

    private:
        //uint16_t _major;
        //uint16_t _minor;
        //uint32_t _patch;
        uint64_t _maj_min_pat;
    };


    constexpr major_minor_patch_version::major_minor_patch_version(
        uint16_t major, uint16_t minor, uint32_t patch) noexcept
        //: _major{ major }, _minor{ minor }, _patch{ patch }
        : _maj_min_pat{ (static_cast<uint64_t>(major) << 48) |
                        (static_cast<uint64_t>(minor) << 32) |
                        static_cast<uint64_t>(patch) }
    {
    }

    constexpr uint16_t major_minor_patch_version::major() const noexcept
    {
        //return _major;
        return (_maj_min_pat & 0xffff000000000000) >> 48;
    }

    constexpr uint16_t major_minor_patch_version::minor() const noexcept
    {
        //return _minor;
        return (_maj_min_pat & 0x0000ffff00000000) >> 32;
    }

    constexpr uint16_t major_minor_patch_version::patch() const noexcept
    {
        //return _patch;
        return (_maj_min_pat & 0x00000000ffffffff);
    }

    constexpr bool major_minor_patch_version::operator==(const major_minor_patch_version& v) noexcept
    {
        return _maj_min_pat == v._maj_min_pat;
    }

    constexpr bool major_minor_patch_version::operator!=(const major_minor_patch_version& v) noexcept
    {
        return _maj_min_pat != v._maj_min_pat;
    }

    constexpr bool major_minor_patch_version::operator>(const major_minor_patch_version& v) noexcept
    {
        return _maj_min_pat > v._maj_min_pat;
    }

    constexpr bool major_minor_patch_version::operator<(const major_minor_patch_version& v) noexcept
    {
        return _maj_min_pat < v._maj_min_pat;
    }

    constexpr bool major_minor_patch_version::operator>=(const major_minor_patch_version& v) noexcept
    {
        return _maj_min_pat >= v._maj_min_pat;
    }

    constexpr bool major_minor_patch_version::operator<=(const major_minor_patch_version& v) noexcept
    {
        return _maj_min_pat <= v._maj_min_pat;
    }


    [[nodiscard]] std::string to_string(major_minor_patch_version v)
    {
        return std::to_string(v.major()) + '.' + std::to_string(v.minor()) + '.' + std::to_string(v.patch());
    }


    [[nodiscard]] major_minor_patch_version major_minor_patch_version::from_string(const std::string& s)
    {
        if (s.size() < 5) // at minimum 5 characters as the format is c.c.c
            throw std::invalid_argument{ DRAKO_STRINGIZE(s) };
        if (std::count(s.begin(), s.end(), '.') != 2) // exactly two dots
            throw std::invalid_argument{ DRAKO_STRINGIZE(s) };

        const auto minor_dot = s.find('.');
        const auto patch_dot = s.rfind('.');

        const auto major = std::stoul(s.substr(0, minor_dot - 1));
        if (major > std::numeric_limits<uint16_t>::max())
            throw std::overflow_error{ "Overflow in major version number." };

        const auto minor = std::stoul(s.substr(minor_dot + 1, patch_dot - 1));
        if (minor > std::numeric_limits<uint16_t>::max())
            throw std::overflow_error{ "Overflow in minor version number." };

        const auto patch = std::stoul(s.substr(patch_dot + 1));
        if (patch > std::numeric_limits<uint32_t>::max())
            throw std::overflow_error{ "Overflow in patch version number." };

        return major_minor_patch_version(major, minor, patch);
    }

    using api_version = major_minor_patch_version;

    // TODO: change 'constexpr' to 'consteval' when is supported
    [[nodiscard]] constexpr api_version build_api_version() noexcept
    {
        /* clang-format off */
        // TODO: configure from CMake
        //return api_version{ ${PROJECT_VERSION_MAJOR}, ${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_PATCH} };
        return api_version{ 0, 1, 0 };
        /* clang-format on */
    }

} // namespace drako


#endif // !DRAKO_API_DEFS_HPP