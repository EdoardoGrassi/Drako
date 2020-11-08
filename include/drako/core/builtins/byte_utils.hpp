#pragma once
#ifndef DRAKO_BYTE_UTILS_HPP
#define DRAKO_BYTE_UTILS_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#if defined(_drako_compiler_msvc)
#include <stdlib.h> // byte swapping intrinsics
#endif

#include <array>
#include <bit>
#include <cstdint>
#include <type_traits>

namespace drako
{
    namespace // impl
    {
        // preserve the endianness of the bytes
        template <typename T, std::size_t... N>
        [[nodiscard]] inline constexpr T _endian_preserve(const std::byte* b) //, std::index_sequence<N...>) noexcept
        {
            std::array<std::byte, sizeof(T)> _bytes{ (b[N], ...) };
            return std::bit_cast<T>(_bytes);
        }

        // swap the endianness of the bytes
        template <typename T, std::size_t... N>
        [[nodiscard]] inline constexpr T _endian_swap(const std::byte* b) //, std::index_sequence<N...>) noexcept
        {
            std::array<std::byte, sizeof(T)> _bytes{ (..., b[N]) }; // from 0 to N-1
            return std::bit_cast<T>(_bytes);
        }

    } // namespace


    template <typename Int> /* clang-format off */
    requires std::is_integral_v<Int> &&
        (std::endian::native == std::endian::little)
    [[nodiscard]] inline constexpr Int from_little_endian(
        const std::array<std::byte, sizeof(Int)> bytes) noexcept /* clang-format on */
    {
        static_assert(alignof(Int) == alignof(decltype(bytes)));
        return std::bit_cast<Int>(bytes);
    }


    template <typename Int> /* clang-format off */
    requires std::is_integral_v<Int> &&
        (std::endian::native == std::endian::little)
    [[nodiscard]] inline constexpr Int from_little_endian(
        const std::byte* bytes) noexcept /* clang-format on */
    {
        return _endian_preserve<Int>(bytes, std::make_index_sequence<sizeof(Int)>{});
    }


    template <typename Int> /* clang-format off */
    requires std::is_integral_v<Int> &&
        (std::endian::native == std::endian::little)
    [[nodiscard]] inline constexpr Int from_big_endian(
        const std::array<std::byte, sizeof(Int)> bytes) noexcept /* clang-format on */
    {
        return _endian_swap<Int>(bytes, std::make_index_sequence<sizeof(Int)>{});
    }


    template <typename Int> /* clang-format off */
    requires std::is_integral_v<Int> &&
        (std::endian::native == std::endian::little)
    [[nodiscard]] inline constexpr Int from_big_endian(
        const std::byte* bytes) noexcept /* clang-format on */
    {
        return _endian_swap<Int, std::make_index_sequence<sizeof(Int)>>(bytes); //, std::make_index_sequence<sizeof(Int)>{});
    }

} // namespace drako

#endif // !DRAKO_BYTE_UTILS_HPP