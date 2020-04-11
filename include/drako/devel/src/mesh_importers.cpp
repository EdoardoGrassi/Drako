#pragma once
#include "drako/devel/mesh_importers.hpp"

#include <filesystem>
#include <iostream>

namespace drako::editor
{
    void import_obj_asset(const project_info& p, const _fs::path& src, const asset_import_context& ctx)
    {
        namespace _obj = drako::file_formats::obj;

        if (!_fs::exists(src) || !_fs::is_regular_file(src))
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
        const auto result = _obj::parse(std::string_view{ data.get(), src_bytes }, {});
        if (const auto report = std::get_if<_obj::parser_result_report>(&result); report)
        {
            for (const auto& object : report->objects)
            {
                const vertex_attribute_descriptor verts_layout[]{
                    // vertex position
                    { meta::format::float_32, vertex_attribute::position, 3 },
                    // vertex normal
                    { meta::format::float_32, vertex_attribute::normal, 3 }
                };
                const vertex_attribute_descriptor index_layout{
                    meta::format::u_int_16,
                    vertex_attribute::index,
                    1
                };
                const auto mesh = from_obj_source(verts_layout, index_layout, object, al);
                ofs << mesh;
            }
        }
        else if (const auto error = std::get_if<_obj::parser_error_report>(&result); error)
        {
            const auto e = *error;
            std::clog << "[DRAKO] File " << src << " at line " << e.line() << ":\n"
                      << "[DRAKO] \terror " << e.value() << ": " << e.message() << '\n';
            return;
        }
    }

} // namespace drako::editor