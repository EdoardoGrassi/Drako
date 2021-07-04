#include "drako/file_formats/png/png.hpp"

#include <bit>
#include <span>
#include <stdexcept>
#include <vector>

namespace drako::formats::png
{
    // from png specs [3.1 png file signature]
    struct _signature
    {
        std::byte sign[8];
    };
    static_assert(sizeof(_signature) == 8);

    // from png specs [3.2 chunck layout]
    struct _chunk_begin_binary
    {
        std::byte length[4];
        std::byte type[4];
    };
    static_assert(sizeof(_chunk_begin_binary) == 8);

    struct _chunk_begin
    {
        std::uint32_t length;
        std::uint32_t type;
    };
    static_assert(sizeof(_chunk_begin) == sizeof(_chunk_begin_binary));

    // from png specs [3.2 chunck layout]
    struct _chunk_end
    {
        std::byte crc32[4];
    };
    static_assert(sizeof(_chunk_end) == 4);

    struct _ihdr_data_binary
    {
        std::byte width[4];
        std::byte height[4];
        std::byte bit_depth;
        std::byte color_type;
        std::byte compression_method;
        std::byte filter_method;
        std::byte interlace_method;
    };
    static_assert(sizeof(_ihdr_data_binary) == 13);


    void _parse_ihdr_chunck(image& image, const _chunk_begin& begin)
    {
    }

    void _parse_plte_chunck(
        image& state, const _chunk_begin& begin, const std::span<std::byte> data)
    {
        if (begin.length % 3 != 0)
            throw ParserError{ "Invalid PLTE chunck length." };

        std::vector<rgb> palette;
        palette.reserve(begin.length);
        for (std::size_t i = 0; i < begin.length; i += 3)
        {
            rgb rgb{
                .r = std::to_integer<std::uint8_t>(data[i]),
                .g = std::to_integer<std::uint8_t>(data[i + 1]),
                .b = std::to_integer<std::uint8_t>(data[i + 2])
            };
            palette.emplace_back(rgb);
        }
    }

    void _parse_idat_chunck(const _chunk_begin& begin)
    {
    }


    void parse(const std::span<std::byte> buffer)
    {
    }
} // namespace drako::formats::png