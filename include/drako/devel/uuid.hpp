#pragma once
#ifndef DRAKO_UUID_HPP
#define DRAKO_UUID_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <variant>

namespace drako
{
    /// @brief Universally unique identifier (UUID).
    class alignas(16) uuid
    {
    public:
        /// @nrief Constructs a null UUID.
        constexpr uuid() noexcept = default;

        /// @brief Constructs an UUID from a byte array.
        explicit constexpr uuid(const std::array<std::byte, 16>&) noexcept;

        /// @brief Construct an UUID from a string representation.
        explicit uuid(const std::string_view);

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

        [[nodiscard]] static uuid parse(const std::string_view s);

    private:
        std::array<std::byte, 16> _bytes = {};
    };
    static_assert(sizeof(uuid) == 16, "Required by RFC specs");
    static_assert(alignof(uuid) == 16, "Required by SIMD implementation");


    constexpr uuid::uuid(const std::array<std::byte, 16>& bytes) noexcept
        : _bytes{ bytes } {}


    [[nodiscard]] bool uuid::has_value() const noexcept
    {
        return std::all_of(_bytes.cbegin(), _bytes.cend(),
            [](auto x) { return x == std::byte{ 0 }; });
    }

    [[nodiscard]] bool operator==(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._bytes.data(), rhs._bytes.data(), sizeof(uuid::_bytes)) == 0;
    }

    [[nodiscard]] bool operator!=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._bytes.data(), rhs._bytes.data(), sizeof(uuid::_bytes)) != 0;
    }

    [[nodiscard]] bool operator>(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._bytes.data(), rhs._bytes.data(), sizeof(uuid::_bytes)) > 0;
    }

    [[nodiscard]] bool operator<(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._bytes.data(), rhs._bytes.data(), sizeof(uuid::_bytes)) < 0;
    }

    [[nodiscard]] bool operator>=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._bytes.data(), rhs._bytes.data(), sizeof(uuid::_bytes)) >= 0;
    }

    [[nodiscard]] bool operator<=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(lhs._bytes.data(), rhs._bytes.data(), sizeof(uuid::_bytes)) <= 0;
    }

    void uuid::reset() noexcept
    {
        std::memset(this, 0, sizeof(uuid::_bytes));
    }

    [[nodiscard]] std::variant<uuid, std::error_code> try_parse(const std::string_view s) noexcept;

} // namespace drako

#endif // !DRAKO_UUID_HPP