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
    class alignas(16) Uuid
    {
    public:
        /// @nrief Constructs a null UUID.
        constexpr Uuid() noexcept = default;

        /// @brief Constructs an UUID from a byte array.
        explicit constexpr Uuid(const std::byte bytes[16]) noexcept
        {
            std::copy_n(bytes, 16, std::begin(_bytes));
        }

        explicit constexpr Uuid(const std::array<std::byte, 16>& bytes) noexcept
            : _bytes{ bytes } {}

        /// @brief Construct an UUID from a string representation.
        explicit Uuid(const std::string_view);

        //Uuid(const Uuid&) noexcept = default;
        //Uuid& operator=(const Uuid&) noexcept = default;

        [[nodiscard]] bool operator==(const Uuid&) const noexcept = default;
        [[nodiscard]] bool operator!=(const Uuid&) const noexcept = default;
        [[nodiscard]] bool operator<(const Uuid&) const noexcept = default;
        [[nodiscard]] bool operator>(const Uuid&) const noexcept = default;
        [[nodiscard]] bool operator<=(const Uuid&) const noexcept = default;
        [[nodiscard]] bool operator>=(const Uuid&) const noexcept = default;

        operator bool() const { return has_value(); }

        /// @brief Resets to null UUID.
        void clear() noexcept { _bytes.fill(std::byte{ 0 }); }

        /// @brief Checks if the UUID is valid.
        [[nodiscard]] bool has_value() const noexcept;

        /// @brief Returns a pointer to the underlying representation.
        [[nodiscard]] std::byte*       data() noexcept { return std::data(_bytes); }
        [[nodiscard]] const std::byte* data() const noexcept { return std::data(_bytes); }

    private:
        std::array<std::byte, 16> _bytes = {};
    };
    static_assert(sizeof(Uuid) == 16, "Bad class layout: internal padding bytes.");
    static_assert(alignof(Uuid) == 16, "Bad class layout: external padding bytes.");


    [[nodiscard]] inline bool Uuid::has_value() const noexcept
    {
        return std::all_of(std::cbegin(_bytes), std::cend(_bytes),
            [](auto x) { return x == std::byte{ 0 }; });
    }

    /*
    [[nodiscard]] inline bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(Uuid::_bytes)) == 0;
    }

    [[nodiscard]] inline bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(Uuid::_bytes)) != 0;
    }

    [[nodiscard]] inline bool operator>(const Uuid& lhs, const Uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(Uuid::_bytes)) > 0;
    }

    [[nodiscard]] inline bool operator<(const Uuid& lhs, const Uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(Uuid::_bytes)) < 0;
    }

    [[nodiscard]] inline bool operator>=(const Uuid& lhs, const Uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(Uuid::_bytes)) >= 0;
    }

    [[nodiscard]] inline bool operator<=(const Uuid& lhs, const Uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(Uuid::_bytes)) <= 0;
    }
    */

    [[nodiscard]] std::string to_string(const Uuid&);

    void parse(const std::string_view s, Uuid& out);

    [[nodiscard]] std::variant<Uuid, std::error_code> try_parse(const std::string_view s) noexcept;

} // namespace drako

#endif // !DRAKO_UUID_HPP