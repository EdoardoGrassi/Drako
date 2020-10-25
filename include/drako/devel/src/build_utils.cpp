#pragma once
#include "drako/devel/build_utils.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"

#include <intrin.h>

namespace drako
{
    /*
    [[nodiscard]] std::uint32_t crc32c(const std::byte* data, const size_t size) noexcept
    {
        DRAKO_ASSERT(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;

        const auto data_as_u8 = reinterpret_cast<const uint8_t*>(data);
        for (size_t i = 0; i < (size % sizeof(uint64_t)); ++i)
            crc = _mm_crc32_u8(crc, data_as_u8[i]);

        const auto data_as_u64 = reinterpret_cast<const uint64_t*>(data + (size % sizeof(uint64_t)));
        for (size_t i = 0; i < (size / sizeof(uint64_t)); ++i)
            crc = _mm_crc32_u64(crc, data_as_u64[i]);

        return crc;
#else
#error Architecture not supported
#endif
    }
    */

    [[nodiscard]] std::uint32_t crc32c(const std::uint32_t* data, const size_t size) noexcept
    {
        DRAKO_ASSERT(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;
        for (size_t i = 0; i < size; ++i)
            crc = _mm_crc32_u32(crc, data[i]);
        return crc;
#else
#error Architecture not supported
#endif
    }

    [[nodiscard]] std::uint32_t crc32c(const std::uint64_t* data, const size_t size) noexcept
    {
        DRAKO_ASSERT(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;
        for (size_t i = 0; i < size; ++i)
            crc = _mm_crc32_u64(crc, data[i]);
        return crc;
#else
#error Architecture not supported
#endif
    }

} // namespace drako