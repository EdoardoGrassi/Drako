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
    namespace
    {
        [[nodiscard]] inline constexpr std::uint64_t _build_version(
            std::uint16_t major, std::uint16_t minor, std::uint32_t patch) noexcept
        {
            const auto maj = static_cast<std::uint64_t>(major) << 48;
            const auto min = static_cast<std::uint64_t>(minor) << 32;
            const auto pat = static_cast<std::uint64_t>(patch);
            return maj | min | pat;
        }

        [[nodiscard]] inline std::uint64_t _safe_parse(const std::string_view s)
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

            return _build_version(major, minor, patch);
        }
    } // namespace


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
            : _maj_min_pat{ _build_version(major, minor, patch) }
        {
        }

        explicit Version(const std::string_view s)
            : _maj_min_pat{ _safe_parse(s) }
        {
        }

        Version(const Version&) noexcept = default;
        Version& operator=(const Version&) noexcept = default;

        [[nodiscard]] constexpr bool operator==(const Version&) const noexcept = default;
        [[nodiscard]] constexpr bool operator!=(const Version&) const noexcept = default;
        [[nodiscard]] constexpr bool operator<(const Version&) const noexcept  = default;
        [[nodiscard]] constexpr bool operator>(const Version&) const noexcept  = default;
        [[nodiscard]] constexpr bool operator<=(const Version&) const noexcept = default;
        [[nodiscard]] constexpr bool operator>=(const Version&) const noexcept = default;

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

        [[nodiscard]] std::string string() const
        {
            return std::to_string(major()) + '.' +
                   std::to_string(minor()) + '.' +
                   std::to_string(patch());
        }

    private:
        //std::uint16_t _major;
        //std::uint16_t _minor;
        //std::uint32_t _patch;
        std::uint64_t _maj_min_pat;
    };
    static_assert(sizeof(Version) == sizeof(std::uint64_t),
        "Class memory requirements: wasted space for padding.");
    static_assert(sizeof(Version) <= alignof(std::uint64_t),
        "Class memory requirements: alignment is excessive.");


    /// @brief Constructs a string with the format [major].[minor].[patch]
    [[nodiscard]] inline std::string to_string(const Version& v)
    {
        return v.string();
    }


    inline void from_string(std::string_view s, Version& v)
    {
        v = Version{ s };
    }

    // TODO: change 'constexpr' to 'consteval' when is supported
    [[nodiscard]] inline constexpr Version current_api_version() noexcept
    {
        /* clang-format off */
        // TODO: configure from CMake
        //return api_version{ 1, 0, 0 };
        return Version{ 0, 1, 0 };
        /* clang-format on */
    }

} // namespace drako


#endif // !DRAKO_API_DEFS_HPP
