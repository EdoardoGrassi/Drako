#include "drako/file_formats/bmp/bmp.hpp"

#include <array>
#include <cstddef>
#include <type_traits>

namespace drako::file_formats::bmp
{
    using _2_bytes = std::array<std::byte, 2>;
    using _4_bytes = std::array<std::byte, 4>;


    // Memory layout of the bitmap file header as binary string.
    struct _file_header_bytes final
    {
        _2_bytes file_type;
        _4_bytes file_size_bytes;
        std::uint8_t : 16; // reserved
        std::uint8_t : 16; // reserved
        _4_bytes data_offset_bytes;
    };
    static_assert(sizeof(_file_header_bytes) == 14,
        "[specs] Header BITMAPFILEHEADER size is 14 bytes.");
    static_assert(alignof(_file_header_bytes) == 1,
        "Alignment requirements enforce a specific placement in the read destination buffer.");
    static_assert(std::is_trivially_copyable_v<_file_header_bytes>);

    struct _file_header final
    {
        explicit _file_header(const _file_header_bytes& h)
            : file_type{ from_little_endian_bytes(h.file_type) }
            , file_size_bytes{ from_little_endian_bytes(h.file_size_bytes) }
            , data_offset_bytes{ from_little_endian_bytes(h.data_offset_bytes) }
        {
        }

        uint32_t file_size_bytes;
        uint32_t data_offset_bytes;
        uint16_t file_type;

        bool validate() noexcept;
    };

    enum class _bits_per_pixel : std::uint16_t
    {
        unspecified = 0,
        bpp_1       = 1,
        bpp_2       = 2,
        bpp_8       = 8,
        bpp_16      = 16,
        bpp_24      = 24,
        bpp_32      = 32
    };

    enum class _compression : std::uint32_t
    {
        uncompressed = 0, // aka BI_RGB value from MS headers
        rle8         = 1, // aka BI_RLE8 value from MS headers
        rl4          = 2, // aka BI_RLE4 value from MS headers
        bitfields    = 3, // aka BI_BITFIELDS value from MS headers
        jpeg_ext     = 4, // aka BI_JPEG value from MS headers (extension)
        png_ext      = 5  // aka BI_PNG value from MS headers (extension)
    };


    struct _core_header_bytes
    {
        _4_bytes header_size_bytes;
        _4_bytes width;
        _4_bytes height;
        _2_bytes planes;
        _2_bytes color_depth;
        _4_bytes compression;
        _4_bytes image_size_bytes;
        _4_bytes horizontal_resolution;
        _4_bytes vertical_resolution;
        _4_bytes palette_colors_count;
        _4_bytes primary_colors_count;
    };
    static_assert(sizeof(_core_header_bytes) == 40,
        "[specs] Header BITMAPCOREHEADER size is 40 bytes.");
    static_assert(alignof(_core_header_bytes) == 1);
    static_assert(std::is_trivially_copyable_v<_core_header_bytes>);

    struct _core_header final
    {
        explicit _core_header(const _core_header_bytes& h)
        {
        }

        uint32_t        header_size_bytes;    // the size of this header (bytes)
        int32_t         width;                // the bitmap width (pixels)
        int32_t         height;               // the bitmap height (pixels)
        uint16_t        planes;               // the number of color planes(must be 1)
        _bits_per_pixel bpp;                  // the color depth of the image (bits per pixel)
        _compression    compression;          // the compression method used
        uint32_t        image_size_bytes;     // the size of the raw bitmap data
        int32_t         h_ppm;                // the horizontal resolution of the image (pixel per metre)
        int32_t         v_ppm;                // the vertical resolution of the image (pixel per metre)
        uint32_t        palette_colors_count; // the numbers of colors in the color palette
        uint32_t        primary_colors_count; // the number of important colors used
    };


    struct color_space_endpoints_t
    {
        _4_bytes Xx;
        _4_bytes Xy;
        _4_bytes Xz;
        _4_bytes Yx;
        _4_bytes Yy;
        _4_bytes Yz;
        _4_bytes Zx;
        _4_bytes Zy;
        _4_bytes Zz;
    };

    struct _v4_header_bytes final
    {
        _core_header_bytes      core;
        _4_bytes                r_mask;
        _4_bytes                g_mask;
        _4_bytes                b_mask;
        _4_bytes                a_mask;
        _4_bytes                color_space_type;
        color_space_endpoints_t color_space_endpoints;
        _4_bytes                r_gamma_curve;
        _4_bytes                g_gamma_curve;
        _4_bytes                b_gamma_curve;
    };
    static_assert(sizeof(_v4_header_bytes) == 108,
        "[specs] Header BITMAPV4HEADER size is 108 bytes.");
    static_assert(alignof(_v4_header_bytes) == 1);
    static_assert(std::is_trivially_copyable_v<_v4_header_bytes>);


    struct _v5_header_bytes final
    {
        _v4_header_bytes base;
        _4_bytes         intent;
        _4_bytes         profile_data_offset;
        _4_bytes         profile_data_size_bytes;
        std::uint8_t : 32; // reserved
    };
    static_assert(sizeof(_v5_header_bytes) == 124,
        "[specs] Header BITMAPV5HEADER size is 124 bytes.");
    static_assert(alignof(_v5_header_bytes) == 1);
    static_assert(std::is_trivially_copyable_v<_v5_header_bytes>);



    parse_result parse(const std::span<std::byte> data)
    {
        const auto         file_header_data = data.data();
        _file_header_bytes file_header_bytes;
        std::memcpy(&file_header_bytes, file_header_data, sizeof(file_header_bytes));
        _file_header file_header{ file_header_bytes };

        if (file_header.file_type != 0x424d) // 'BM'
            throw parser_error{ "File signature doesn't match .bmp signature." };
        if (file_header.file_size_bytes != data.size_bytes())
            throw parser_error{ "File lenght doesn't match provided data buffer." };

        const auto         core_header_data = file_header_data + sizeof(file_header_bytes);
        _core_header_bytes core_header_bytes;
        std::memcpy(&core_header_bytes, core_header_data, sizeof(core_header_bytes));
        _core_header core_header{ core_header_bytes };

        if (core_header.header_size_bytes == sizeof(_v4_header_bytes))
            throw;
        else if (core_header.header_size_bytes == sizeof(_v5_header_bytes))
            throw;

        throw std::runtime_error{ "Not implemented" };
    }

} // namespace drako::file_formats::bmp