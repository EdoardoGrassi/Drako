#include "drako/devel/mesh_importers.hpp"

#include "drako/file_formats/dson/dson.hpp" // NOTE: for the time being we use Drakoson format
#include "drako/file_formats/wavefront/object.hpp"
#include "drako/file_formats/wavefront/parser.hpp"

#include <filesystem>
#include <fstream>

namespace drako::editor
{
    void import_obj_file(
        const std::filesystem::path& src,
        const std::filesystem::path& dst,
        const properties& p, const flags& f)
    {
        namespace _fs  = std::filesystem;
        namespace _obj = drako::file_formats::obj;

        if (!_fs::is_regular_file(src))
            throw std::invalid_argument{ DRAKO_STRINGIZE(src) };
        if (!_fs::is_directory(dst))
            throw std::invalid_argument{ DRAKO_STRINGIZE(src) };

        std::ifstream ifs{ src };
        if (!ifs)
        {
            std::clog << "[DRAKO] Couldn't open " << src << '\n';
            return;
        }

        std::ofstream ofs{ dst, std::ofstream::trunc };
        if (!ofs)
        {
            std::clog << "[DRAKO] Couldn't open " << dst << '\n';
            return;
        }

        const auto src_bytes = _fs::file_size(src);
        const auto data      = std::make_unique<char[]>(src_bytes);
        ifs.read(data.get(), src_bytes);

        // const _obj::parser_config config{};
        const auto result = _obj::parse({ data.get(), src_bytes }, {});
        for (const auto& object : result.objects)
        {
            std::clog << "Processing object " << object.name << '\n';
            if (!std::empty(object.faces))
            {
                // worst case scenario: each data point is never reused
                const auto size   = sizeof(float) * 3 * std::size(object.faces);
                auto       buffer = std::make_unique<std::byte[]>(size);
            }
        }


        throw std::runtime_error{ "Not implemented." };
    }

} // namespace drako::editor