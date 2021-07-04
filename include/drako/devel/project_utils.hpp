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
    /// @brief Serialize project metadata,
    //void save(const std::filesystem::path&, const ProjectMetaInfo&);

    /// @brief Deserialize project metadata.
    //void load(const std::filesystem::path&, ProjectMetaInfo&);
    //void load(const std::filesystem::path&, std::size_t, ProjectMetaInfo&);

    /// @brief Serialize project data.
    //void save(const std::filesystem::path&, const ProjectDatabase&);

    /// @brief Deserialize project data.
    //void load(const std::filesystem::path&, ProjectDatabase&);

    /// @brief Serialize asset metadata.
    //void save(const std::filesystem::path&, const AssetImportInfo&);

    /// @brief Deserialize asset metadata.
    //void load(const std::filesystem::path&, AssetImportInfo&);
    //void load(const std::filesystem::path&, std::size_t, AssetImportInfo&);


    inline void load_by_path(const std::filesystem::path& file, AssetImportInfo& info)
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
    }


    //void load_asset_index_cache(const Project& p);


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

} // namespace drako::editor

#endif // !DRAKO_PROJECT_UTILS_HPP