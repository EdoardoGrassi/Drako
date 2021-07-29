#pragma once
#ifndef DRAKO_MESH_IMPORTERS_HPP
#define DRAKO_MESH_IMPORTERS_HPP

#include "drako/devel/project_types.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/mesh_utils.hpp"

#include <uuid-cpp/uuid.hpp>

#include <filesystem>

namespace drako::editor
{
    struct MeshImportInfo
    {
        uuid::Uuid            guid;
        std::string           name;
        std::filesystem::path path;
    };

    //const dson::DOM& operator>>(const dson::DOM&, MeshImportInfo&);
    //dson::DOM&       operator<<(dson::DOM&, const MeshImportInfo&);


    struct ObjImportConfig
    {
        std::vector<std::pair<std::string, std::string>> props;
        std::vector<std::string>                         flags;
    };

    // TODO: change string flags to typed flags

    /// @brief Import a mesh asset from an .obj file.
    ///
    /// Optional configuration settings:
    ///     --discard-normals   remove normals data from output
    ///     --discard-uvs       remove texture uvs data from output
    ///
    [[nodiscard]] Mesh compile(const obj::MeshData& md, const ObjImportConfig& config);

    /// @brief Import a mesh asset from an .obj file.
    [[nodiscard]] Mesh compile(const obj::MeshData& md);

    /// @brief Import an .obj file as a mesh.
    void import_asset_obj(const ProjectContext& c, const AssetImportInfo&, const AssetImportContext&);

} // namespace drako::editor

#endif // !DRAKO_MESH_IMPORTERS_HPP