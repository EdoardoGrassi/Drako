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
    struct MeshImportInfo
    {
        uuid::Uuid            guid;
        std::string           name;
        std::filesystem::path path;
    };

    const dson::DOM& operator>>(const dson::DOM&, MeshImportInfo&);
    dson::DOM&       operator<<(dson::DOM&, const MeshImportInfo&);


    struct ObjImportConfig
    {
        std::vector<std::pair<std::string, std::string>> props;
        std::vector<std::string>                         flags;
    };

    /// @brief Import assets from .obj file.
    ///
    /// Possible configuration settings:
    ///     --discard-normals   remove normals data from output
    ///     --discard-uvs       remove texture uvs data from output
    ///
    [[nodiscard]] Mesh compile(const obj::MeshData& obj, const ObjImportConfig& config);

    [[nodiscard]] Mesh compile(const obj::MeshData& md, bool discard_normals);

} // namespace drako::editor

#endif // !DRAKO_MESH_IMPORTERS_HPP