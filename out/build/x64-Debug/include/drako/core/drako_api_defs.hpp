/// @file
/// @brief  Core header with Drako definitions.
/// @author Grassi Edoardo

#pragma once
#ifndef DRAKO_API_DEFS_HPP
#define DRAKO_API_DEFS_HPP

#include "drako/core/platform.hpp"

#include <charconv>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>

// Drako base namespace.
//#define DRAKO_HPP_NAMESPACE drako


namespace drako
{
    /// @brief Three-part version identification number.
    ///
    /// Composed of major, minor and patch numbers (as in major.minor.patch).
    ///
    class Version
    {
    public:
        constexpr explicit Version(
            std::uint16_t major, std::uint16_t minor, std::uint32_t patch) noexcept
            //: _major{ major }, _minor{ minor }, _patch{ patch }
            : _maj_min_pat{ (static_cast<std::uint64_t>(major) << 48) |
                            (static_cast<std::uint64_t>(minor) << 32) |
                            static_cast<std::uint64_t>(patch) }
        {
        }

        Version(const Version&) noexcept = default;
        Version& operator=(const Version&) noexcept = default;

        [[nodiscard]] friend constexpr bool operator==(Version, Version) noexcept = default;
        [[nodiscard]] friend constexpr bool operator!=(Version, Version) noexcept = default;
        [[nodiscard]] friend constexpr bool operator>(Version, Version) noexcept  = default;
        [[nodiscard]] friend constexpr bool operator<(Version, Version) noexcept  = default;
        [[nodiscard]] friend constexpr bool operator>=(Version, Version) noexcept = default;
        [[nodiscard]] friend constexpr bool operator<=(Version, Version) noexcept = default;

        /// @brief Major number.
        [[nodiscard]] constexpr std::uint16_t major() const noexcept
        {
            return (_maj_min_pat & 0xffff000000000000) >> 48;
        }

        /// @brief Minor number.
        [[nodiscard]] constexpr std::uint16_t minor() const noexcept
        {
            //return _minor;
            return (_maj_min_pat & 0x0000ffff00000000) >> 32;
        }

        /// @brief Patch number.
        [[nodiscard]] constexpr std::uint16_t patch() const noexcept
        {
            //return _patch;
            return (_maj_min_pat & 0x00000000ffffffff);
        }

    private:
        //std::uint16_t _major;
        //std::uint16_t _minor;
        //std::uint32_t _patch;
        std::uint64_t _maj_min_pat;
    };
    static_assert(sizeof(Version) == sizeof(std::uint64_t));


    /// @brief Constructs a string with the format [major].[minor].[patch]
    [[nodiscard]] inline std::string to_string(Version v)
    {
        return std::to_string(v.major()) +
               '.' + std::to_string(v.minor()) +
               '.' + std::to_string(v.patch());
    }


    [[nodiscard]] inline void from_string(std::string_view s, Version& v)
    {
        if (std::size(s) < 5) // at minimum 5 characters as the format is c.c.c
            throw std::runtime_error{ "Malformed version number." };
        if (std::count(std::cbegin(s), std::cend(s), '.') != 2) // exactly two dots
            throw std::runtime_error{ "Malformed version number." };

        const auto minor_dot = s.find('.');
        const auto patch_dot = s.rfind('.');

        std::uint16_t major{};
        {
            const auto first = std::data(s);
            const auto last  = std::data(s) + minor_dot - 1;
            if (const auto r = std::from_chars(first, last, major); r.ec != std::errc{})
                throw std::runtime_error{ "Malformed <major> version number." };
        }

        std::uint16_t minor{};
        {
            const auto first = std::data(s) + minor_dot + 1;
            const auto last  = std::data(s) + patch_dot - 1;
            if (const auto r = std::from_chars(first, last, minor); r.ec != std::errc{})
                throw std::runtime_error{ "Malformed <minor> version number." };
        }

        std::uint32_t patch{};
        {
            const auto first = std::data(s) + patch_dot + 1;
            const auto last  = std::data(s) + std::size(s);
            if (const auto r = std::from_chars(first, last, patch); r.ec != std::errc{})
                throw std::runtime_error{ "Malformed <patch> version number." };
        }

        v = Version{ major, minor, patch };
    }

    // TODO: change 'constexpr' to 'consteval' when is supported
    [[nodiscard]] inline constexpr Version current_api_version() noexcept
    {
        /* clang-format off */
        // TODO: configure from CMake
        //return api_version{ 0, 1, 0 };
        return Version{ 0, 1, 0 };
        /* clang-format on */
    }

} // namespace drako


#endif // !DRAKO_API_DEFS_HPP
