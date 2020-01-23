#pragma once
#ifndef DRAKO_ENDIANNESS_HPP
#define DRAKO_ENDIANNESS_HPP

#include <cstdint>
#include <limits>
#include <type_traits>
#include <intrin.h>

#include "drako/devel/assertion.hpp"

namespace drako::meta
{
    // Transforms data to the endianness used by the processor
    //
    template <typename Res>
    inline constexpr Res native_endian(const Res val) noexcept = delete;


    template <>
    inline constexpr uint32_t native_endian(uint32_t val) noexcept
    {
        if constexpr (/* BIG ENDIAN */ true)
        {
            return impl::swap_endian(val);
        }
        else if constexpr (/* LITTLE ENDIAN */ true)
        {
            return val;
        }
        else
        {
            #error "Cannot determine endianness for target processor"
        }
    }

    // Performs copy of raw memory segments and correctly handles endianness.
    //
    template <typename Res>
    void memcpy_endian(void* dest, const void* src, size_t count);

    template <typename Res>
    void memcpy_aligned_endian();

    template <>
    void memcpy_endian<int32_t>(void* dest, const void* src, size_t count)
    {
        DRAKO_ASSERT_NOT_NULL(dest);
        DRAKO_ASSERT_NOT_NULL(src);
        DRAKO_ASSERT(count >= 16);

        auto reg = _mm512_loadu_epi32(src);
        //const __m256i shuffle_control = _mm256_set_epi8()
        reg = _mm512_or_epi32(_mm512_srli_epi32(reg, CHAR_BIT), _mm512_slli_epi32(reg, CHAR_BIT));
        reg = _mm512_or_epi32(_mm512_srli_epi32(reg, CHAR_BIT * 2), _mm512_slli_epi32(reg, CHAR_BIT * 2));
        // TODO: end impl
    }

    namespace impl
    {
        inline constexpr uint32_t swap_endian(uint32_t val) noexcept
        {
            val = ((val >> 8) & 0x00FF00FF | (val << 8) & 0xFF00FF00);
            val = ((val >> 16) & 0x0000FFFF | (val << 16) & 0xFFFF0000);
            return val;
        }

        inline constexpr uint64_t swap_endian(uint64_t val) noexcept
        {
            val = ((val >> 8) & 0x00FF00FF00FF00FF | (val << 8) & 0xFF00FF00FF00FF00);
            val = ((val >> 16) & 0x0000FFFF0000FFFF | (val << 16) & 0xFFFF0000FFFF0000);
            val = ((val >> 32) & 0x00000000FFFFFFFF | (val << 32) & 0xFFFFFFFF00000000);
            return val;
        }

        inline constexpr void swap_endian_inplace(uint32_t* pVal) noexcept
        {
            // TODO:
        }

        inline constexpr void swap_endian_inplace(uint64_t* pVal) noexcept
        {
            // TODO:
        }
    }

} // namespace drako::meta

#endif // !DRAKO_ENDIANNESS_HPP
