#pragma once
#ifndef DRAKO_MESH_IMPORTERS_HPP
#define DRAKO_MESH_IMPORTERS_HPP

#include "drako/devel/project_types.hpp"
#include "drako/file_formats/dson.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/mesh_utils.hpp"

#include <filesystem>
#include <fstream>

namespace drako::editor
{
    struct mesh_import_info
    {
    public:
        uuid                  guid;
        std::string           name;
        std::filesystem::path path;
    };


    mesh_import_info load_mesh_import(const std::filesystem::path& file)
    {
        file_formats::dson::object serialized{};
        {
            std::ifstream ifs{ file };
            ifs >> serialized;
        }

        mesh_import_info info;
        info.guid = uuid::parse(serialized.get("guid"));
        info.name = serialized.get("name");
        info.path = serialized.get("path");
        return info;
    }

    void save_mesh_import(const mesh_import_info& info, const std::filesystem::path& file)
    {
        file_formats::dson::object serialized;
        serialized.set("guid", to_string(info.guid));
        serialized.set("name", info.name);
        serialized.set("path", info.path.string());

        std::ofstream ofs{ file };
        ofs << serialized;
    }


    // import assets from .OBJ format files
    void import_obj_asset(const project_info& p, const std::filesystem::path& src, const asset_import_context& ctx);

} // namespace drako::editor

#endif // !DRAKO_MESH_IMPORTERS_HPP