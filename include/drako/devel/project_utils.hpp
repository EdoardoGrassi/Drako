#pragma once
#ifndef DRAKO_PROJECT_UTILS_HPP
#define DRAKO_PROJECT_UTILS_HPP

#include "drako/devel/project_types.hpp"

#include <uuid-cpp/uuid.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

namespace drako::editor
{
    /*inline void load_by_path(const std::filesystem::path& file, AssetImportInfo& info)
    {
        std::ifstream ifs{ file };
        dson::DOM     dom{};
        ifs >> dom;
        dom >> info;
    }

    inline void save_by_path(const std::filesystem::path& file, const AssetImportInfo& info)
    {
        std::ofstream ofs{ file };
        dson::DOM     dom{};
        dom << info;
        ofs << dom;
    }*/


    /// @brief Creates the filesystem tree for a new project.
    ///
    /// Generates the project structure on the filesystem,
    /// with main directory <name> located inside <where> folder.
    ///
    /// @param[in] ctx  Project context.
    /// @param[in] name Name of the project.
    ///
    void create_project_tree(const ProjectContext& ctx, std::string_view name);

    /// @brief Create and import a new asset.
    //void create_asset(const ProjectContext&, ProjectDatabase&, const std::filesystem::path&, const AssetImportInfo&);

    /// @brief Perform import process of an existing asset.
    //void import_asset(const ProjectContext& ctx, const Uuid&);

    //void load_all_assets(const ProjectContext& ctx, ProjectDatabase& db);

    //void compile_as_native(const ProjectContext& ctx, );

    void import(const ProjectContext& ctx, const AssetImportInfo& info);

} // namespace drako::editor

#endif // !DRAKO_PROJECT_UTILS_HPP