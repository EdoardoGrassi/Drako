#pragma once
#ifndef DRAKO_UUID_HPP_
#define DRAKO_UUID_HPP_

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <variant>

#if defined(DRAKO_API_SIMD)

#if defined(DRAKO_CC_MSVC)
#include <intrin.h> // MSVC intrinsics header

#elif defined(DRAKO_CC_GCC)
#include <x86intrin.h> // GCC intrinsics header
#endif

#endif

namespace drako
{
    // Universally unique identifier.
    class alignas(16) uuid
    {
    public:
        // Constructs an invalid UUID.
        explicit constexpr uuid() noexcept;

        // Constructs an UUID from a byte array.
        explicit constexpr uuid(const std::array<std::byte, 16>&) noexcept;

        // Constructs an UUID from a character string.
        // constexpr uuid(const char[37]) noexcept;

        uuid(const uuid&) noexcept = default;
        uuid& operator=(const uuid&) noexcept = default;

        friend bool operator==(const uuid&, const uuid&) noexcept;
        friend bool operator!=(const uuid&, const uuid&) noexcept;
        friend bool operator>(const uuid&, const uuid&) noexcept;
        friend bool operator<(const uuid&, const uuid&) noexcept;
        friend bool operator>=(const uuid&, const uuid&) noexcept;
        friend bool operator<=(const uuid&, const uuid&) noexcept;

        // Reset to invalid UUID.
        void reset() noexcept;

        // Check if UUID is valid.
        [[nodiscard]] bool has_value() const noexcept;

        friend std::istream& operator>>(std::istream&, uuid&);
        friend std::ostream& operator<<(std::ostream&, const uuid&);

        friend uuid make_uuid_version1();

        friend std::string to_string(const uuid&);

    private:
#if !defined(DRAKO_API_SIMD)
        std::byte _data[16] = { std::byte{ 0 } };

        // std::uint32_t _time_low;
        // std::uint16_t _time_mid;
        // std::uint16_t _time_hi_and_version;
        // std::uint8_t  _clock_seq_hi_and_reserved;
        // std::uint8_t  _clock_seq_low;
        // std::uint8_t  _node[6];

        // static_assert(sizeof(_node) == 6, "Required by RFC specs: node field must be 48 bits");
#else
        __m128i _data;
#endif
    };
    static_assert(sizeof(uuid) == 16, "Required by RFC specs");
    static_assert(alignof(uuid) == 16, "Required by SIMD implementation");

    constexpr uuid::uuid() noexcept
    {
    }

    constexpr uuid::uuid(const std::array<std::byte, 16>& b) noexcept
        : _data{ b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7],
            b[8], b[9], b[10], b[11], b[12], b[13], b[14], b[15] }
    {
    }

    [[nodiscard]] bool uuid::has_value() const noexcept
    {
        return *this != uuid{};

#if defined(DRAKO_API_SIMD)
        // TODO: impl
#endif
    }

    [[nodiscard]] bool operator==(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._data, rhs._data, sizeof(uuid)) == 0;

#if defined(DRAKO_API_SIMD)
        // TODO: impl
#endif
    }

    [[nodiscard]] bool operator!=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._data, rhs._data, sizeof(uuid)) != 0;

#if defined(DRAKO_API_SIMD)
        // TODO: impl
#endif
    }

    [[nodiscard]] bool operator>(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._data, rhs._data, sizeof(uuid)) > 0;
    }

    [[nodiscard]] bool operator<(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._data, rhs._data, sizeof(uuid)) < 0;
    }

    [[nodiscard]] bool operator>=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._data, rhs._data, sizeof(uuid)) >= 0;
    }

    [[nodiscard]] bool operator<=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._data, rhs._data, sizeof(uuid)) <= 0;
    }

    void uuid::reset() noexcept
    {
        std::memset(this, 0, sizeof(uuid));
    }

    [[nodiscard]] std::variant<uuid, std::error_code> parse(const std::string_view s) noexcept;

} // namespace drako

#endif // !DRAKO_UUID_HPP_