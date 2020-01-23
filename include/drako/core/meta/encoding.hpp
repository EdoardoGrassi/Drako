#pragma once
#ifndef DRAKO_ENCODING_HPP
#define DRAKO_ENCODING_HPP

#include <cstdint>

namespace drako::meta
{
    enum class format : std::uint8_t
    {
        // es. FLOAT_32, FLOAT_64, U_INT_8, U_INT_16, U_INT_32, U_INT_64 ...
        DK_FORMAT_INT8,
        DK_FORMAT_UINT8,
        DK_FORMAT_INT16,
        DK_FORMAT_UINT16,
        INT_16,
        UINT_32,
        INT_32,
        UINT_64,
        INT_64
    };
} // namespace drako::meta

#endif // !DRAKO_ENCODING_HPP