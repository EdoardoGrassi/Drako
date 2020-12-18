#pragma once
#ifndef DRAKO_MESH_IMPORTERS_HPP
#define DRAKO_MESH_IMPORTERS_HPP

#include "drako/devel/project_types.hpp"
#include "drako/file_formats/dson/dson.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/mesh_utils.hpp"

#include <filesystem>
#include <fstream>

namespace drako::editor
{
    struct mesh_import_info
    {
        Uuid                  guid;
        std::string           name;
        std::filesystem::path path;
    };


    [[nodiscard]] inline mesh_import_info load_mesh_import(const std::filesystem::path& file)
    {
        dson::DOM serialized{};
        {
            std::ifstream ifs{ file };
            ifs >> serialized;
        }

        mesh_import_info info;
        parse(serialized.get("guid"), info.guid);
        info.name = serialized.get("name");
        info.path = serialized.get("path");
        return info;
    }

    inline void save_mesh_import(const mesh_import_info& info, const std::filesystem::path& file)
    {
        dson::DOM serialized;
        serialized.set("guid", to_string(info.guid));
        serialized.set("name", info.name);
        serialized.set("path", info.path.string());

        std::ofstream ofs{ file };
        ofs << serialized;
    }

    using properties = std::vector<std::pair<std::string, std::string>>;
    using flags      = std::vector<std::string>;

    /// @brief Import assets from .obj file.
    ///
    /// Possible configuration settings:
    ///     --discard-normals   remove normals data from output
    ///     --discard-uvs       remove texture uvs data from output
    ///
    void import_obj_file(
        const std::filesystem::path& src,
        const std::filesystem::path& dst,
        const properties& p, const flags& f);

} // namespace drako::editor

#endif // !DRAKO_MESH_IMPORTERS_HPP