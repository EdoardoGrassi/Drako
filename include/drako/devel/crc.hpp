#pragma once
#ifndef DRAKO_CRC_HPP
#define DRAKO_CRC_HPP

#include <span>
#include <string>

namespace drako
{
    [[nodiscard]] std::uint32_t crc32(std::span<const std::byte> data) noexcept;

    /// @brief Cyclic redundancy check (CRC-32/Castagnoli version).
    [[nodiscard]] std::uint32_t crc32c(std::span<const std::byte> data) noexcept;

    /// @brief Cyclic redundancy check (CRC-32/Castagnoli version).
    [[nodiscard]] std::uint32_t crc32c(std::span<const std::byte> data, std::uint32_t crc) noexcept;

    /// @brief Cyclic redundancy check (CRC-32/Castagnoli version).
    [[nodiscard]] std::uint32_t crc32c(std::span<const std::uint32_t> data) noexcept;

    /// @brief Cyclic redundancy check (CRC-32/Castagnoli version).
    [[nodiscard]] std::uint32_t crc32c(std::span<const std::uint64_t> data) noexcept;

    [[nodiscard]] inline std::uint32_t crc32c(const std::string_view s) noexcept
    {
        const std::span<const char> span{ std::data(s), std::size(s) };
        return crc32c(std::as_bytes(span));
    }

    class Crc32c
    {
    public:

        void reset() noexcept { _crc = 0xffffffff; }

    private:
        std::uint32_t _crc = 0xffffffff;
    };

} // namespace drako

#endif // !DRAKO_CRC_HPP