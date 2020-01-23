#include "drako/plugins/bmp_format_parser.hpp"

namespace drako::plugins
{
    void parse(std::byte* data) noexcept
    {
        const auto file_header = reinterpret_cast<_bitmap_file_header*>(data);


        const auto color_table = ();

        const auto pixel_array = static_cast<std::uint32_t*>();
    }

} // namespace drako::plugins