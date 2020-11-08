#pragma once
#ifndef DRAKO_FORMATS_PNG_HPP
#define DRAKO_FORMATS_PNG_HPP

#include <stdexcept>
#include <vector>

namespace drako::formats::png
{
    /// @brief Color encoded as RGB tuple.
    struct rgb
    {
        std::uint8_t r, g, b;
    };

    /// @brief Exception emitted on parsing errors.
    class parser_error : public std::runtime_error
    {
    public:
        explicit parser_error(const char* msg)
            : std::runtime_error(msg)
        {
        }
    };


    struct image
    {
        std::vector<rgb> palette;
    };


    class parser
    {
    };
} // namespace drako::formats::png

#endif // !DRAKO_FORMATS_PNG_HPP