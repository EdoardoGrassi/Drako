#pragma once
#ifndef DRAKO_PROJECT_TYPES_HPP
#define DRAKO_PROJECT_TYPES_HPP

#include "drako/core/drako_api_defs.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/file_formats/dson/dson.hpp"

#include <uuid-cpp/uuid.hpp>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <string>
#include <unordered_map>


namespace drako::editor
{
    const std::filesystem::path project_asset_dir{ "/assets" };
    const std::filesystem::path project_cache_dir{ "/cache" };
    const std::filesystem::path project_meta_dir{ "/meta" };

    //
    // Main project file located at the root of the project tree.
    //
    const std::filesystem::path project_config_file = "project_config.drako.proj";

    /// @brief Metadata of a project.
    struct ProjectManifest
    {
        std::string           name;
        std::filesystem::path root;
        std::string           author;
        Version               editor_version = current_api_version();
    };

    const dson::DOM& operator>>(const dson::DOM&, ProjectManifest&);
    dson::DOM&       operator<<(dson::DOM&, const ProjectManifest&);

    const YAML::Node& operator>>(const YAML::Node&, ProjectManifest&);
    YAML::Node&       operator<<(YAML::Node&, const ProjectManifest&);


    /// @brief External asset descriptor.
    ///
    /// Holds information for the import process of an extern file.
    ///
    struct AssetImportInfo
    {
        uuid::Uuid            id;
        std::filesystem::path path;
        std::string           name;
        Version               version = current_api_version();

        [[nodiscard]] bool operator==(const AssetImportInfo&) const = default;
        [[nodiscard]] bool operator!=(const AssetImportInfo&) const = default;
    };

    const dson::DOM& operator>>(const dson::DOM&, AssetImportInfo&);
    dson::DOM&       operator<<(dson::DOM&, const AssetImportInfo&);


    struct AssetImportError
    {
        std::filesystem::path asset;
        std::string           message;
    };


    struct AssetImportContext
    {
        std::unordered_map<std::string, std::string> properties;
        std::vector<std::string>                     flags;
    };


    /// @brief Project instance.
    class ProjectContext
    {
    public:
        explicit ProjectContext(const std::filesystem::path& root);

        explicit ProjectContext(const ProjectManifest& pm);

        ProjectContext(const ProjectContext&) = delete;
        ProjectContext& operator=(const ProjectContext&) = delete;

        ProjectContext(ProjectContext&&) = delete;
        ProjectContext& operator=(ProjectContext&&) = delete;


        /// @brief Root folder of a project.
        [[nodiscard]] std::filesystem::path root() const { return _root; }

        /// @brief Main asset folder.
        [[nodiscard]] std::filesystem::path asset_directory() const { return _root / "assets"; }

        /// @brief Cached datafiles folder.
        [[nodiscard]] std::filesystem::path cache_directory() const { return _root / "cache"; }

        /// @brief Main metafiles folder.
        [[nodiscard]] std::filesystem::path meta_directory() const { return _root / "meta"; }

        /// @brief Map an asset id to the corresponding datafile path.
        [[nodiscard]] std::filesystem::path guid_to_datafile(const uuid::Uuid&) const;

        /// @brief Map an asset id to the corresponding metafile path.
        [[nodiscard]] std::filesystem::path guid_to_metafile(const uuid::Uuid&) const;

        /// @brief Generates a new UUID.
        [[nodiscard]] uuid::Uuid generate_asset_uuid() const noexcept { return _engine(); }

        uuid::Uuid import(const std::filesystem::path& asset);

        struct AssetScanResult
        {
            std::vector<AssetImportInfo>  assets;
            std::vector<AssetImportError> errors;
        };
        /// @brief Load all metadata for assets referenced by a project.
        [[nodiscard]] AssetScanResult scan_all_assets() const;

    private:
        std::filesystem::path _root;   // Root of the project tree.
        uuid::SystemEngine    _engine; // Reusable generator for uuids.

        // deserialize through a common function
        template <typename T>
        static void _load_by_path(const std::filesystem::path& p, T& t)
        {
            dson::DOM dom;
            std::ifstream{ p } >> dom;
            dom >> t;
        }

        // serialize through a common function
        template <typename T>
        static void _save_by_path(const std::filesystem::path& p, const T& t)
        {
            dson::DOM dom;
            dom << t;
            std::ofstream{ p } << dom;
        }
    };


    /*
    /// @brief Internal asset descriptor.
    ///
    /// Holds information about an asset stored as engine-ready file.
    ///
    struct internal_asset_info
    {
        /// @brief Globally unique identifier of the asset.
        Uuid id;

        /// @brief Type of content.
        asset_type type;

        /// @brief Human readable name of the asset.
        std::string name;

        /// @brief Version of the editor used for the serialization.
        Version version = current_api_version();
    };

    //std::istream& load(std::istream&, internal_asset_info&);
    //std::ostream& save(std::ostream&, const internal_asset_info&);

    dson::DOM& operator>>(dson::DOM&, internal_asset_info&);
    dson::DOM& operator<<(dson::DOM&, const internal_asset_info&);
    */


    using AssetImportFunction = void (*)(const ProjectContext&, const std::filesystem::path&, const AssetImportContext&);

    /// @brief Stack of objects that interact with assets on import.
    class AssetImportStack
    {
    public:
        AssetImportStack(const std::unordered_map<std::string, AssetImportFunction>& importers)
            : _importers{ importers } {}

        /// @brief Retrieve the importer associated to a file extension.
        [[nodiscard]] AssetImportFunction importer(const std::string& extension)
        {
            return _importers.at(extension);
        }

    private:
        std::unordered_map<std::string, AssetImportFunction> _importers;
    };



    /// @brief Project database object.
    class ProjectDatabase
    {
    public:
        explicit ProjectDatabase(const ProjectContext&);

        /// @brief Insert a new asset.
        /// @param asset Path of the asset file.
        /// @param info  Asset import manifest.
        void insert_asset(const std::filesystem::path& asset, const AssetImportInfo& info);

        void package_as_single_bundle(const std::filesystem::path& where);

    private:
        /// @brief Database-like table of available assets.
        struct _asset_table_t
        {
            /// @brief Assets uuids.
            std::vector<uuid::Uuid> ids;

            /// @brief Assets names.
            std::vector<std::string> names;

            /// @brief Assets full paths on local disk.
            std::vector<std::filesystem::path> paths;

            /// @brief Assets binary size on local disk.
            std::vector<std::size_t> sizes;
        } _assets;
    };

} // namespace drako::editor

#endif // !DRAKO_PROJECT_TYPES_HPP