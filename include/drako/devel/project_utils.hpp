#pragma once
#ifndef DRAKO_PROJECT_UTILS_HPP
#define DRAKO_PROJECT_UTILS_HPP

#include "drako/devel/asset_types.hpp"
#include "drako/devel/project_types.hpp"

#include <uuid-cpp/uuid.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

namespace drako::editor
{
    /// @brief Retrive an asset metafile name from its guid.
    [[nodiscard]] inline std::filesystem::path guid_to_metafile(const AssetID& id)
    {
        assert(id);
        return to_string(id) + ".dkmeta.yaml";
    }

    /// @brief Retrive an asset datafile name from its guid.
    [[nodiscard]] inline std::filesystem::path guid_to_datafile(const AssetID& id)
    {
        assert(id);
        return to_string(id) + ".dkdata";
    }



    /// @brief Path to asset binary data directory.
    [[nodiscard]] inline std::filesystem::path
    asset_data_directory(const std::filesystem::path& root)
    {
        assert(std::filesystem::is_directory(root));
        return root / "assets-data";
    }

    /// @brief Path to asset metadata directory.
    [[nodiscard]] inline std::filesystem::path
    asset_meta_directory(const std::filesystem::path& root)
    {
        assert(std::filesystem::is_directory(root));
        return root / "assets-meta";
    }

    /// @brief Path to bundle binary data directory.
    [[nodiscard]] inline std::filesystem::path
    bundle_data_directory(const std::filesystem::path& root)
    {
        assert(std::filesystem::is_directory(root));
        return root / "bundles-data";
    }

    /// @brief Path to bundle metadata directory.
    [[nodiscard]] inline std::filesystem::path
    bundle_meta_directory(const std::filesystem::path& root)
    {
        assert(std::filesystem::is_directory(root));
        return root / "bundles-meta";
    }



    /// @brief Path to asset binary data directory.
    [[nodiscard]] inline std::filesystem::path
    asset_data_directory(const ProjectContext& pc)
    {
        return asset_data_directory(pc.root());
    }

    /// @brief Path to asset metadata directory.
    [[nodiscard]] inline std::filesystem::path
    asset_meta_directory(const ProjectContext& pc)
    {
        return asset_meta_directory(pc.root());
    }

    /// @brief Path to bundle binary data directory.
    [[nodiscard]] inline std::filesystem::path
    bundle_data_directory(const ProjectContext& pc)
    {
        return bundle_data_directory(pc.root());
    }

    /// @brief Path to bundle metadata directory.
    [[nodiscard]] inline std::filesystem::path
    bundle_meta_directory(const ProjectContext& pc)
    {
        return bundle_meta_directory(pc.root());
    }



    /// @brief Builds the fullpath to an asset datafile.
    ///
    [[nodiscard]] inline std::filesystem::path
    asset_data_path(const ProjectContext& pc, const AssetID& id)
    {
        return asset_data_directory(pc) / guid_to_datafile(id);
    }

    /// @brief Builds the fullpath to an asset metafile.
    ///
    [[nodiscard]] inline std::filesystem::path
    asset_meta_path(const ProjectContext& pc, const AssetID& id)
    {
        return asset_meta_directory(pc) / guid_to_metafile(id);
    }


    // deserialize through a common function
    template <typename T>
    inline void load_text_format(const std::filesystem::path& p, T& t)
    {
        const auto yaml = YAML::LoadFile(p.string());
        yaml >> t;
    }

    // serialize through a common function
    template <typename T>
    inline void save_text_format(const std::filesystem::path& p, const T& t)
    {
        YAML::Node yaml{};
        yaml << t;
        std::ofstream{ p } << yaml;
    }


    /// @brief Create the metafile associated with an asset.
    ///
    /// @param[in] c Target project.
    /// @param[in] i Asset metadata descriptor.
    ///
    void create_asset_meta(
        const ProjectContext& c, const AssetImportInfo& i);


    /// @brief Destroy the metafile associated with an asset.
    ///
    /// @param[in] c Target project.
    /// @param[in] i Asset metadata descriptor.
    ///
    void destroy_asset_meta(
        const ProjectContext& c, const AssetImportInfo& i);


    /// @brief Destroy the datafile associated with an asset.
    ///
    /// @param[in] c Target project.
    /// @param[in] i Asset metadata descriptor.
    ///
    void destroy_asset_data(
        const ProjectContext& c, const AssetImportInfo& i);


    /// @brief Import an external file into a project.
    ///
    /// @param[in] c Target project.
    /// @param[in] f Asset import function to use.
    /// @param[in] i Asset metadata descriptor.
    ///
    void import_asset_data(
        const ProjectContext& c, const AssetImportFunction& f, const AssetImportInfo& i);


    /// @brief Make an asset usable by the engine.
    /// @param[in] info Descriptor of the asset.
    ///
    /// Compile an asset into a format suitable for consumption by the engine.
    ///
    void compile(const AssetImportInfo& info);


    struct AssetScanResult
    {
        std::vector<AssetImportInfo>  assets;
        std::vector<AssetImportError> errors;
    };
    /// @brief Load metadata for all the assets in the project tree.
    [[nodiscard]] AssetScanResult scan_all_assets(const ProjectContext&);

    struct BundleScanResult
    {
        std::vector<AssetBundleInfo>        bundles;
        std::vector<AssetBundleImportError> errors;
    };
    /// @brief Load metadata for all the bundles in the project tree.
    [[nodiscard]] BundleScanResult scan_all_bundles(const ProjectContext&);


    /// @brief Package all assets in the project as a single bundle
    [[nodiscard]] AssetBundleManifest package_as_single(const ProjectContext&, const ProjectDatabase&);


    /// @brief Creates the filesystem tree for a new project.
    ///
    /// Generates the project structure on the filesystem,
    /// with main directory <name> located inside <where> folder.
    ///
    /// @param[in] ctx  Project context.
    /// @param[in] name Name of the project.
    ///
    //void create_project_tree(const ProjectContext& ctx, std::string_view name);

    /// @brief Create and import a new asset.
    //void create_asset(const ProjectContext&, ProjectDatabase&, const std::filesystem::path&, const AssetImportInfo&);

    /// @brief Perform import process of an existing asset.
    //void import_asset(const ProjectContext& ctx, const Uuid&);

    //void load_all_assets(const ProjectContext& ctx, ProjectDatabase& db);

    //void compile_as_native(const ProjectContext& ctx, );

} // namespace drako::editor

#endif // !DRAKO_PROJECT_UTILS_HPP