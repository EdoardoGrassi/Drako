#include "drako/devel/build_utils.hpp"

#include "drako/core/platform.hpp"

#include <intrin.h>

#include <cassert>
#include <cstddef>

namespace drako
{
    [[nodiscard]] std::uint32_t crc32c(const std::byte* data, std::size_t size) noexcept
    {
        assert(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;

        const auto offset = size % sizeof(std::uint64_t);

        // remove unaligned bytes at the front
        const auto unaligned = reinterpret_cast<const unsigned char*>(data);
        for (std::size_t i = 0; i < offset; ++i)
            crc = _mm_crc32_u8(crc, unaligned[i]);

        // process batches of aligned bytes
        const auto aligned = reinterpret_cast<const std::uint64_t*>(data + offset);
        for (std::size_t i = 0; i < ((size - offset) / sizeof(std::uint64_t)); ++i)
            crc = _mm_crc32_u64(crc, aligned[i]);

        return crc;
#else
#error Architecture not supported.
#endif
    }

    [[nodiscard]] std::uint32_t crc32c(const std::uint32_t* data, std::size_t size) noexcept
    {
        assert(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;
        for (std::size_t i = 0; i < size; ++i)
            crc = _mm_crc32_u32(crc, data[i]);
        return crc;
#else
#error Architecture not supported.
#endif
    }

    [[nodiscard]] std::uint32_t crc32c(const std::uint64_t* data, std::size_t size) noexcept
    {
        assert(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;
        for (std::size_t i = 0; i < size; ++i)
            crc = _mm_crc32_u64(crc, data[i]);
        return crc;
#else
#error Architecture not supported.
#endif
    }

} // namespace drako