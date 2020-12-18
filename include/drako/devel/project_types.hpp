#pragma once
#ifndef DRAKO_PROJECT_TYPES_HPP
#define DRAKO_PROJECT_TYPES_HPP

#include "drako/core/drako_api_defs.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/uuid.hpp"
#include "drako/devel/uuid_engine.hpp"
#include "drako/file_formats/dson/dson.hpp"

#include <filesystem>
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

    //
    // Project tree structure:
    //
    // ROOT
    // |- project_config.drako.proj
    // |- ./assets
    // |- ./cache
    // |- ./meta
    //

    /// @brief Metadata of a project.
    struct ProjectMetaInfo
    {
        std::string name;
        //std::string author;
        Version version = current_api_version();
    };

    //std::istream& operator>>(std::istream&, ProjectMetaInfo&);
    //std::ostream& operator<<(std::ostream&, const ProjectMetaInfo&);

    const dson::DOM& operator>>(const dson::DOM&, ProjectMetaInfo&);
    dson::DOM&       operator<<(dson::DOM&, const ProjectMetaInfo&);


    /// @brief Project instance.
    class ProjectContext
    {
    public:
        explicit ProjectContext(const std::filesystem::path& root)
            : _root{ root } {}

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
        [[nodiscard]] std::filesystem::path guid_to_datafile(const Uuid&) const;

        /// @brief Map an asset id to the corresponding metafile path.
        [[nodiscard]] std::filesystem::path guid_to_metafile(const Uuid&) const;

        /// @brief Generates a new UUID.
        [[nodiscard]] Uuid generate_asset_uuid() const noexcept { return _engine(); }

    private:
        std::filesystem::path _root;   // Root of the project tree.
        UuidMacEngine         _engine; // Reusable generator for uuids.
    };


    /// @brief Project database object.
    struct ProjectDatabase
    {
        /// @brief Database-like table of available assets.
        struct asset_table_t
        {
            /// @brief Assets uuids.
            std::vector<Uuid> ids;

            /// @brief Assets names.
            std::vector<std::string> names;

            /// @brief Assets full paths on local disk.
            std::vector<std::filesystem::path> paths;

            /// @brief Assets binary size on local disk.
            std::vector<std::size_t> sizes;
        } assets;
    };

    void package_assets_as_single_bundle(
        const ProjectDatabase& db, const std::filesystem::path& where);

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


    /// @brief External asset descriptor.
    ///
    /// Holds information for the import process of an extern file.
    ///
    struct AssetImportInfo
    {
        Uuid                  id;
        std::filesystem::path path;
        std::string           name;
        Version               version = current_api_version();
    };

    //std::istream& load(std::istream&, AssetImportInfo&);
    //std::ostream& save(std::ostream&, const AssetImportInfo&);

    const dson::DOM& operator>>(const dson::DOM&, AssetImportInfo&);
    dson::DOM&       operator<<(dson::DOM&, const AssetImportInfo&);


    struct AssetImportContext
    {
        std::unordered_map<std::string, std::string> properties;
        std::vector<std::string>                     flags;
    };

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

} // namespace drako::editor

#endif // !DRAKO_PROJECT_TYPES_HPP