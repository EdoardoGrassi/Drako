#pragma once
#ifndef DRAKO_MESH_IMPORTERS_HPP_
#define DRAKO_MESH_IMPORTERS_HPP_

#include "drako/devel/project_types.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/mesh_utils.hpp"

#include <fstream>

namespace drako::editor
{
    namespace _fs = std::filesystem;

    struct mesh_import_info
    {
    public:
        uuid        guid;
        std::string name;
        _fs::path   path;
    };


    mesh_import_info load_mesh_import(const _fs::path& file)
    {
        const auto       deserialized = file_formats::drakoson::from_stream(std::ifstream{ file });
        mesh_import_info info;
        info.guid = parse(deserialized.at("guid"));
        info.name = deserialized.at("name");
        info.path = deserialized.at("path");
        return info;
    }

    void save_mesh_import(const mesh_import_info& info, const _fs::path& file)
    {
        file_formats::drakoson::object serialized;
        serialized["guid"] = to_string(info.guid);
        serialized["name"] = info.name;
        serialized["path"] = info.path.string();
        file_formats::drakoson::to_stream(serialized, std::ofstream{ file });
    }


    // import assets from .OBJ format files
    void import_obj_asset(const project_info& p, const _fs::path& src, const asset_import_context& ctx);

} // namespace drako::editor

#endif // !DRAKO_MESH_IMPORTERS_HPP_