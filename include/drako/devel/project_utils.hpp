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
    /// @param file Source file path
    ///
    [[nodiscard]] internal_asset_info make_asset_metafile(const fs::path& file);


    /// @brief Load metadata of an asset.
    /// @param file Metadata file path
    ///
    [[nodiscard]] internal_asset_info load_asset_metafile(const fs::path& file);

    void load_asset_index_cache(const project& p);

    /// @brief Creates the filesystem tree for a new project.
    ///
    /// Generates the project structure on the filesystem,
    /// with main directory <name> located inside <where> folder.
    ///
    /// @param name  Name of the project.
    /// @param where Directory where the project is generated.
    /// 
    /// @pre @p where must already exist.
    ///
    void make_project_tree(std::string_view name, const fs::path& where);


    /// @brief Create an imported asset instance.
    ///
    /// @param p   Project info
    /// @param src Input file for the importer
    ///
    void import_external_asset(const project_info& p, const fs::path& src);


    /// @brief Create an imported asset instance.
    ///
    /// @param p       Project info
    /// @param sources Input files for the importer
    ///
    void import_external_asset(const project_info& p, const std::vector<fs::path>& sources);


    struct load_importable_result
    {
    };

    [[nodiscard]] load_importable_result load_imported_asset(const project_info& p, const uuid& guid);


    ///
    /// @brief Load project base information.
    ///
    /// @param root Project root directory
    ///
    [[nodiscard]] project_info load_project_info(const fs::path& root);

    ///
    /// @brief Load full project data from persistent storage.
    ///
    /// @param project Project.
    ///
    void load_project(project& project);

    ///
    /// @brief Save all project data to persisent storage.
    ///
    /// @param project Project.
    ///
    void save_project(const project& project);

    ///
    /// @brief Retrieves the full path to an asset.
    ///
    /// @param p     Project header
    /// @param asset Asset GUID
    ///
    [[nodiscard]] fs::path guid_to_path(const project_info& p, const uuid& asset);


} // namespace drako::editor

#endif // !DRAKO_PROJECT_UTILS_HPP