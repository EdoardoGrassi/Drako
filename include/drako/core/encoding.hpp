#pragma once
#ifndef DRAKO_ENCODING_HPP
#define DRAKO_ENCODING_HPP

#include <cstdint>

namespace drako::meta
{
    enum class format : std::uint8_t
    {
        // es. FLOAT_32, FLOAT_64, U_INT_8, U_INT_16, U_INT_32, U_INT_64 ...
        s_int_8,
        u_int_8,
        s_int_16,
        u_int_16,
        s_int_32,
        u_int_32,
        s_int_64,
        u_int_64,
        float_32,
        float_64
    };

    [[nodiscard]] inline std::size_t _sizeof(format f) noexcept
    {
        switch (f)
        {
            case format::s_int_8:
            case format::u_int_8:
                return sizeof(std::int8_t);

            case format::s_int_16:
            case format::u_int_16:
                return sizeof(std::uint16_t);

            case format::s_int_32:
            case format::u_int_32:
                return sizeof(std::uint32_t);

            default:
                std::exit(EXIT_FAILURE);
        }
    }

} // namespace drako::meta

#endif // !DRAKO_ENCODING_HPP