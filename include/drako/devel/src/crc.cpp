#include "drako/devel/crc.hpp"

#include "drako/core/platform.hpp"

#include <intrin.h>

#include <cassert>
#include <cstddef>
#include <memory>
#include <span>

namespace drako
{
    // Reference for crc32 implementation:
    // https://zlib.net/crc_v3.txt

    template <std::uint32_t Poly, std::uint32_t InitXor, std::uint32_t FinalXor>
    [[nodiscard]] std::uint32_t _base_crc32(std::span<const std::byte> bytes) noexcept
    {
        // based on Sarwate's algorithm

        std::uint32_t table[256];
        for (auto i = 0; i < 256; ++i)
        {
            std::uint32_t reg = i;
            for (auto bit = 0; bit < 8; ++bit)
            {
                reg = (reg & 1) ? (reg >> 1) ^ Poly : (reg >> 1);
            }
            table[i] = reg;
        }

        std::uint32_t crc = InitXor;

        // least significant bits first
        for (auto b : bytes)
            crc = table[(crc ^ b) & 0x000000ff] ^ (crc >> 8);
        return crc ^ FinalXor;
    }

    // from https://reveng.sourceforge.io/crc-catalogue/17plus.htm
    // CRC-32/ISCSI
    // width = 32
    // poly = 0x1edc6f41
    // init = 0xffffffff
    // refin = true
    // refout = true
    // xorout = 0xffffffff
    // check = 0xe3069283
    // residue = 0xb798b438
    // name = "CRC-32/ISCSI"
    [[nodiscard]] std::uint32_t crc32c(std::span<const std::byte> bytes, std::uint32_t crc) noexcept
    {
        assert(!std::empty(bytes));

#if defined(DRAKO_ARCH_INTEL)
        using _u64 = std::uint64_t;

        for (auto i = 0; i < std::size(bytes) / sizeof(_u64); ++i)
        {
            _u64 aligned;
            std::memcpy(&aligned, &bytes[i * sizeof(_u64)], sizeof(_u64));
            crc = _mm_crc32_u64(crc, aligned);
        }
        for (auto i = 0; i < std::size(bytes) % sizeof(_u64); ++i)
            crc = _mm_crc32_u8(crc, std::to_integer<unsigned char>(bytes[i]));

        //for (auto b : bytes)
        //    crc = _mm_crc32_u8(crc, std::to_integer<unsigned char>(b));

        /*
        const auto rawp = reinterpret_cast<std::uintptr_t>(std::data(bytes));
        const auto next_aligned_p = rawp / alignof(_u64) * (alignof(_u64) + 1);
        const auto head_bytes_size = std::distance(std::data(bytes), next_aligned_p);
        assert(head_bytes_size < alignof(_u64));
        assert(head_bytes_size < sizeof(_u64)); // less than an aligned block

        // process unaligned bytes at the front
        for (const auto byte : bytes.first(head_bytes_size))
            crc = _mm_crc32_u8(crc, static_cast<unsigned char>(byte));

        const auto aligned_blocks = (std::size(bytes) - head_bytes_size) / sizeof(_u64);

        // process batches of aligned bytes
        auto aligned = bytes.subspan(head_bytes_size, aligned_blocks * sizeof(_u64));
        // check alignment of remaining bytes
        assert(reinterpret_cast<std::uintptr_t>(std::data(aligned)) % alignof(_u64) == 0);

        for (std::size_t i = 0; i < (std::size(aligned) / sizeof(_u64)); i += sizeof(_u64))
        {
            const auto aligned_bytes = std::assume_aligned<alignof(_u64)>(bytes[head_bytes_size + i]);
            crc                      = _mm_crc32_u64(crc, std::bit_cast<_u64>(aligned_bytes));
        }

        // process unaligned bytes at the end
        const auto tail_bytes_size = std::size(bytes) - head_bytes_size - std::size(aligned) * sizeof(_u64);
        assert(tail_bytes_size < sizeof(_u64)); // less than an aligned block
        for (const auto byte : bytes.last(tail_bytes_size))
            crc = _mm_crc32_u8(crc, static_cast<unsigned char>(byte));
        */

        return crc;
#else
#error Architecture not supported.
#endif
    }

    [[nodiscard]] std::uint32_t crc32c(std::span<const std::byte> bytes) noexcept
    {
        constexpr auto _xor = std::numeric_limits<std::uint32_t>::max();
        return crc32c(bytes, _xor) ^ _xor;
    }

    [[nodiscard]] std::uint32_t crc32c(std::span<const std::uint32_t> data, std::uint32_t crc) noexcept
    {
        assert(!std::empty(data));

#if defined(DRAKO_ARCH_INTEL)
        for (const auto& u32 : data)
            crc = _mm_crc32_u32(crc, u32);
        return crc;
#else
#error Architecture not supported.
#endif
    }

    [[nodiscard]] std::uint32_t crc32c(std::span<const std::uint32_t> data) noexcept
    {
        constexpr auto _xor = std::numeric_limits<std::uint32_t>::max();
        return crc32c(data, _xor) ^ _xor;
    }

    [[nodiscard]] std::uint32_t crc32c(std::span<const std::uint64_t> data, std::uint32_t crc) noexcept
    {
        assert(!std::empty(data));

#if defined(DRAKO_ARCH_INTEL)
        static_assert(std::endian::native == std::endian::little);
        for (const auto& u64 : data)
            crc = _mm_crc32_u64(crc, u64);
        return crc;
#else
#error Architecture not supported.
#endif
    }

    [[nodiscard]] std::uint32_t crc32c(std::span<const std::uint64_t> data) noexcept
    {
        constexpr auto _xor = std::numeric_limits<std::uint32_t>::max();
        return crc32c(data, _xor) ^ _xor;
    }

} // namespace drako