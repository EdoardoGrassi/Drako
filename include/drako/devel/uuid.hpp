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
        explicit constexpr uuid(const std::array<std::byte, 16>& bytes) noexcept
            : _bytes{ bytes } {}

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

        /// @brief Resets to null UUID.
        void clear() noexcept;

        /// @brief Checks if the UUID is valid.
        [[nodiscard]] bool has_value() const noexcept;

        /// @brief Returns a pointer to the underlying representation.
        [[nodiscard]] std::byte*       data() noexcept { return std::data(_bytes); }
        [[nodiscard]] const std::byte* data() const noexcept { return std::data(_bytes); }

        friend std::string to_string(const uuid&);

        [[nodiscard]] static uuid parse(const std::string_view s);

    private:
        std::array<std::byte, 16> _bytes = {};
    };
    static_assert(sizeof(uuid) == 16, "Bad class layout: internal padding bytes.");
    static_assert(alignof(uuid) == 16, "Bad class layout: external padding bytes.");


    [[nodiscard]] inline bool uuid::has_value() const noexcept
    {
        return std::all_of(std::cbegin(_bytes), std::cend(_bytes),
            [](auto x) { return x == std::byte{ 0 }; });
    }

    [[nodiscard]] inline bool operator==(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(uuid)) == 0;
    }

    [[nodiscard]] inline bool operator!=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(uuid)) != 0;
    }

    [[nodiscard]] inline bool operator>(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(uuid)) > 0;
    }

    [[nodiscard]] inline bool operator<(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(uuid)) < 0;
    }

    [[nodiscard]] inline bool operator>=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(uuid)) >= 0;
    }

    [[nodiscard]] inline bool operator<=(const uuid& lhs, const uuid& rhs) noexcept
    {
        return std::memcmp(std::data(lhs), std::data(rhs), sizeof(uuid)) <= 0;
    }

    void inline uuid::clear() noexcept
    {
        std::memset(data(), 0, sizeof(_bytes));
    }

    [[nodiscard]] std::variant<uuid, std::error_code> try_parse(const std::string_view s) noexcept;

} // namespace drako

#endif // !DRAKO_UUID_HPP