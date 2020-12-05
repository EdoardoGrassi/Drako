#pragma once
#ifndef DRAKO_PROJECT_UTILS_HPP
#define DRAKO_PROJECT_UTILS_HPP

#include "drako/devel/project_types.hpp"
#include "drako/devel/uuid.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem; // convenient alias

namespace drako::editor
{

    /// @brief Generate metadata file for an asset.
    ///
    /// @param[in] file Source file path
    ///
    [[nodiscard]] internal_asset_info make_asset_metafile(const fs::path& file);


    /// @brief Load metadata of an asset.
    ///
    /// @param[in] file Metadata file path
    ///
    [[nodiscard]] internal_asset_info load_asset_metafile(const fs::path& file);

    void load_asset_index_cache(const Project& p);


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
    void make_project_tree(std::string_view name, const fs::path& where);


    /// @brief Create an imported asset instance.
    ///
    /// @param[in] p   Project info
    /// @param[in] src Input file for the importer
    ///
    void import_external_asset(const project_info& p, const fs::path& src);


    /// @brief Create an imported asset instance.
    ///
    /// @param[in] p       Project info
    /// @param[in] sources Input files for the importer
    ///
    void import_external_asset(const project_info& p, const std::vector<fs::path>& sources);


    struct load_importable_result
    {
    };
    [[nodiscard]] load_importable_result load_imported_asset(const project_info& p, const Uuid& asset);


    /// @brief Load project base information.
    ///
    /// @param[in] root Project root directory
    ///
    [[nodiscard]] project_info load_project_info(const fs::path& root);


    /// @brief Load full project data from persistent storage.
    ///
    /// @param[in] p Project.
    ///
    void load_project(Project& p);


    /// @brief Save all project data to persisent storage.
    ///
    /// @param[in] p Project.
    ///
    void save_project(const Project& p);


    /// @brief Retrieves the full path to an asset.
    ///
    /// @param[in] p     Project header
    /// @param[in] asset Asset GUID
    ///
    [[nodiscard]] fs::path guid_to_path(const project_info& p, const Uuid& asset);


} // namespace drako::editor

#endif // !DRAKO_PROJECT_UTILS_HPP