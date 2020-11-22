#pragma once
#ifndef DRAKO_BYTE_UTILS_HPP
#define DRAKO_BYTE_UTILS_HPP

#include "drako/core/intrinsics.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <type_traits>

namespace drako
{
    /// @brief Read an integer from little endian bytes.
    template <typename Int> /* clang-format off */
    requires std::is_integral_v<Int> && (std::endian::native == std::endian::little)
    [[nodiscard]] inline constexpr Int from_le_bytes(
        const std::array<std::byte, sizeof(Int)>& bytes) noexcept /* clang-format on */
    {
        static_assert(alignof(Int) == alignof(decltype(bytes)));
        return std::bit_cast<Int>(bytes);
    }

    /// @brief Read an integer from little endian bytes.
    template <typename Int> /* clang-format off */
    requires std::is_integral_v<Int> && (std::endian::native == std::endian::little)
    [[nodiscard]] inline constexpr Int from_le_bytes(
        const std::byte* bytes) noexcept /* clang-format on */
    {
        std::array<std::byte, sizeof(Int)> temp;
        std::memcpy(&temp, bytes, sizeof(temp));
        return std::bit_cast<Int>(temp);
    }


    /// @brief Read an integer from big endian bytes.
    template <typename Int> /* clang-format off */
    requires std::is_integral_v<Int> && (std::endian::native == std::endian::little)
    [[nodiscard]] inline constexpr Int from_be_bytes(
        const std::array<std::byte, sizeof(Int)>& bytes) noexcept /* clang-format on */
    {
        static_assert(alignof(Int) == alignof(decltype(bytes)));
        return byte_swap(std::bit_cast<Int>(bytes));
    }


    /// @brief Read an integer from big endian bytes.
    template <typename Int> /* clang-format off */
    requires std::is_integral_v<Int> && (std::endian::native == std::endian::little)
    [[nodiscard]] inline constexpr Int from_be_bytes(
        const std::byte* bytes) noexcept /* clang-format on */
    {
        std::array<std::byte, sizeof(Int)> temp;
        std::memcpy(&temp, bytes, sizeof(temp));
        return byte_swap(std::bit_cast<Int>(temp));
    }

} // namespace drako

#endif // !DRAKO_BYTE_UTILS_HPP