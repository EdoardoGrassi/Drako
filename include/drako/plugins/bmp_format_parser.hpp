#pragma once
#ifndef DRAKO_BMP_FORMAT_PARSER_HPP
#define DRAKO_BMP_FORMAT_PARSER_HPP

#include <cstdint>
#include <memory>

namespace drako::plugins
{
    struct _bitmap_file_header final
    {
        uint16_t file_type;
        uint32_t file_size_bytes;
        uint16_t : 16; // reserved
        uint16_t : 16; // reserved
        uint32_t data_offset_bytes;

        bool validate() noexcept;
    };
    static_assert(sizeof(_bitmap_file_header) == 14, "Header size must be 14 bytes");

    enum class _bmp_bits_per_pixel : std::uint16_t
    {
        unspecified = 0,
        bpp_1       = 1,
        bpp_2       = 2,
        bpp_8       = 8,
        bpp_16      = 16,
        bpp_24      = 24,
        bpp_32      = 32
    };

    enum class _bmp_compression : std::uint32_t
    {
        uncompressed = 0, // aka BI_RGB value from MS headers
        rle8         = 1, // aka BI_RLE8 value from MS headers
        rl4          = 2, // aka BI_RLE4 value from MS headers
        bitfields    = 3, // aka BI_BITFIELDS value from MS headers
        jpeg         = 4, // aka BI_JPEG value from MS headers
        png          = 5  // aka BI_PNG value from MS headers
    };

    struct _bitmap_info_header final
    {
        uint32_t            header_size_bytes;
        int32_t             width_px;
        int32_t             height_px;
        uint16_t            planes;
        _bmp_bits_per_pixel bpp;
        _bmp_compression    compression;
        uint32_t            image_size_bytes;
        int32_t             h_ppm;
        int32_t             v_ppm;
        uint32_t            total_colors_count;
        uint32_t            _colors_count;
    };
    static_assert(sizeof(_bitmap_info_header) == 40, "Header size must be 40 bytes");

    struct _bitmap_v4_header final
    {
        _bitmap_info_header core;
        uint32_t            r_mask;
        uint32_t            g_mask;
        uint32_t            b_mask;
        uint32_t            a_mask;
        DWORD               bV4CSType;
        CIEXYZTRIPLE        bV4Endpoints;
        uint32_t            r_gamma_curve;
        uint32_t            g_gamma_curve;
        uint32_t            b_gamma_curve;
    };
    static_assert(sizeof(_bitmap_v4_header) == 108, "Header size must be 108 bytes");

    struct _bitmap_v5_header final
    {
    };
    static_assert(sizeof(_bitmap_v5_header) == 124, "Header size must be 124 bytes");

    void parse(std::byte* data) noexcept;

} // namespace drako::plugins

#endif // !DRAKO_BMP_FORMAT_PARSER_HPP