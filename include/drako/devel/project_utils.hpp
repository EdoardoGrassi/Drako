#pragma once
#ifndef DRAKO_PROJECT_UTILS_HPP
#define DRAKO_PROJECT_UTILS_HPP

#include "drako/devel/project_types.hpp"
#include "drako/devel/uuid.hpp"

#include <filesystem>
#include <vector>

namespace drako::editor
{
    /// @brief Insert an asset in the project database.
    void insert(ProjectDatabase&, const std::filesystem::path&, const AssetImportInfo&);


    /// @brief Serialize project metadata,
    void save(const std::filesystem::path&, const ProjectMetaInfo&);

    /// @brief Deserialize project metadata.
    void load(const std::filesystem::path&, ProjectMetaInfo&);

    /// @brief Serialize project data.
    void save(const std::filesystem::path&, const ProjectDatabase&);

    /// @brief Deserialize project data.
    void load(const std::filesystem::path&, ProjectDatabase&);

    /// @brief Serialize asset metadata.
    void save(const std::filesystem::path&, const AssetImportInfo&);

    /// @brief Deserialize asset metadata.
    void load(const std::filesystem::path&, AssetImportInfo&);


    //void load_asset_index_cache(const Project& p);


    /// @brief Creates the filesystem tree for a new project.
    ///
    /// Generates the project structure on the filesystem,
    /// with main directory <name> located inside <where> folder.
    ///
    /// @param[in] name  Name of the project.
    /// @param[in] where Directory where the project is generated.
    ///
    /// @pre @p where must already exist.
    ///
    void create_project_tree(std::string_view name, const std::filesystem::path& where);

    /// @brief Create and import a new asset.
    void create_asset(const ProjectContext&, ProjectDatabase&, const std::filesystem::path&, const AssetImportInfo&);

    /// @brief Perform import process of an existing asset.
    void import_asset(const ProjectContext& ctx, const Uuid&);

} // namespace drako::editor

#endif // !DRAKO_PROJECT_UTILS_HPP