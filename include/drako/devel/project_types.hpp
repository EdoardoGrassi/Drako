#pragma once
#ifndef DRAKO_PROJECT_TYPES_HPP
#define DRAKO_PROJECT_TYPES_HPP

#include "drako/devel/asset_bundle_types.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/version.hpp"

#include <uuid-cpp/uuid.hpp>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <unordered_map>


namespace drako::editor
{
    /// @brief Metadata of a project.
    struct ProjectManifest
    {
        std::string name;
        std::string author;
        Version     editor_version = current_api_version();

        [[nodiscard]] bool operator==(const ProjectManifest&) const& = default;
        [[nodiscard]] bool operator!=(const ProjectManifest&) const& = default;
    };

    const YAML::Node& operator>>(const YAML::Node&, ProjectManifest&);
    YAML::Node&       operator<<(YAML::Node&, const ProjectManifest&);


    /// @brief External asset descriptor.
    ///
    /// Holds information for the import process of an extern file.
    ///
    struct AssetImportInfo
    {
        uuid::Uuid            guid;
        std::filesystem::path path;
        std::string           name;
        Version               version = current_api_version();

        [[nodiscard]] bool operator==(const AssetImportInfo&) const& = default;
        [[nodiscard]] bool operator!=(const AssetImportInfo&) const& = default;
    };

    const YAML::Node& operator>>(const YAML::Node&, AssetImportInfo&);
    YAML::Node&       operator<<(YAML::Node&, const AssetImportInfo&);


    struct AssetImportError
    {
        std::filesystem::path file;
        std::string           message;
    };


    struct AssetImportContext
    {
        std::unordered_map<std::string, std::string> properties;
        std::vector<std::string>                     flags;
    };


    /// @brief Project database object.
    struct ProjectDatabase
    {
        /// @brief Database-like table of available assets.
        struct _asset_table_t
        {
            /// @brief Assets uuids.
            std::vector<uuid::Uuid> guids;

            /// @brief Assets names.
            std::vector<std::string> names;

            /// @brief Assets full paths on local disk.
            std::vector<std::filesystem::path> paths;

            /// @brief Assets binary size on local disk.
            //std::vector<std::size_t> sizes;
        } assets;
    };


    /// @brief Project instance.
    class ProjectContext
    {
    public:
        /// @brief Opens an existing project.
        ///
        /// @param root Root directory of the project.
        explicit ProjectContext(const std::filesystem::path& root);

        /// @brief Creates a new project.
        ///
        /// @param root Root directory of the project.
        /// @param pm   Project description.
        explicit ProjectContext(const std::filesystem::path& root, const ProjectManifest& pm);

        ProjectContext(const ProjectContext&) = default;
        ProjectContext& operator=(const ProjectContext&) = default;

        //ProjectContext(ProjectContext&&) = delete;
        //ProjectContext& operator=(ProjectContext&&) = delete;


        /// @brief Root folder of a project.
        [[nodiscard]] std::filesystem::path root() const { return _root; }

        /// @brief Map an asset id to the corresponding datafile path.
        [[nodiscard]] std::filesystem::path guid_to_datafile(const AssetID&) const;

        /// @brief Map an asset id to the corresponding metafile path.
        [[nodiscard]] std::filesystem::path guid_to_metafile(const AssetID&) const;

        /// @brief Generates a new UUID.
        [[nodiscard]] uuid::Uuid generate_asset_uuid() const noexcept { return _engine(); }

        //uuid::Uuid track(const std::filesystem::path& asset);

    private:
        ProjectManifest       _meta;   // project metadata
        std::filesystem::path _root;   // root of the project tree
        uuid::SystemEngine    _engine; // reusable generator for uuids
    };


    //using AssetImportFunction = void (*)(const ProjectContext&, const AssetImportInfo&, const AssetImportContext&);
    using AssetImportFunction = std::function<void(const ProjectContext&, const AssetImportInfo&, const AssetImportContext&)>;

    /// @brief Stack of objects that interact with assets on import.
    /* class AssetImportStack
    {
    public:
        AssetImportStack(const std::unordered_map<std::string, AssetImportFunction>& importers)
            : _importers{ importers } {}

        /// @brief Retrieve the importer associated to a file extension.
        [[nodiscard]] AssetImportFunction importer(const std::string& extension) const
        {
            return _importers.at(extension);
        }

    private:
        std::unordered_map<std::string, AssetImportFunction> _importers;
    }; */

    using AssetImportStack = std::map<std::string, AssetImportFunction>;


} // namespace drako::editor

#endif // !DRAKO_PROJECT_TYPES_HPP