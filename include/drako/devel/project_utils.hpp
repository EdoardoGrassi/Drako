#pragma once
#ifndef DRAKO_PROJECT_UTILS_HPP_
#define DRAKO_PROJECT_UTILS_HPP_

#include "drako/devel/project_types.hpp"
#include "drako/devel/uuid.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem; // convenient alias

namespace drako::editor
{

    //
    // @brief Generate metadata file for an asset.
    // @param file Source file path
    //
    [[nodiscard]] internal_asset_info make_meta_info(const project_info& p, const fs::path& file);

    //
    // @brief Load metadata of an asset.
    // @param file Metadata file path
    //
    [[nodiscard]] internal_asset_info load_meta_info(const project_info& p, const fs::path& file);


    //
    // @brief Create project directory tree.
    //
    // @param root Path of project root folder
    //
    void make_project_tree(const fs::path& root);


    //
    // @brief Create an imported asset instance.
    //
    // @param p   Project info
    // @param src Input file for the importer
    //
    void import_external_asset(const project_info& p, const fs::path& src);

    //
    // @brief Create an imported asset instance.
    //
    // @param p       Project info
    // @param sources Input files for the importer
    //
    void import_external_asset(const project_info& p, const std::vector<fs::path>& sources);


    struct load_importable_result
    {
    };

    [[nodiscard]] load_importable_result load_imported_asset(const project_info& p, const uuid& guid);

    //
    // @brief Load project base information.
    //
    // @param root Project root directory
    //
    [[nodiscard]] project_info load_project_info(const fs::path& root);


    struct project
    {
        std::vector<uuid>          asset_uuids;
        std::vector<std::string>   asset_names;
        std::vector<std::uint32_t> asset_sizes;

        std::vector<uuid> meshes;

        std::vector<uuid> forced_asset_inclusion;
    };

    //
    // @brief Load full project data.
    // @param project Project description
    //
    [[nodiscard]] project load_project(const project_info& p);

    // [[nodiscard]] build_error build_devel_project(const project_info& p);


    //
    // @brief Retrieves the full path to an asset.
    //
    // @param p     Project header
    // @param asset Asset GUID
    //
    [[nodiscard]] fs::path guid_to_path(const project_info& p, const uuid& asset);

} // namespace drako::editor

#endif // !DRAKO_PROJECT_UTILS_HPP_