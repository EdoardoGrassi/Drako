#pragma once
#ifndef DRAKO_PROJECT_TYPES_HPP
#define DRAKO_PROJECT_TYPES_HPP

#include "drako/core/drako_api_defs.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/uuid.hpp"

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

    struct project_info
    {
        api_version version = build_api_version();
        std::string name;
        std::string author;
        //_fs::file_time_type last_project_scan;
    };

    std::istream& operator>>(std::istream&, project_info&);
    std::ostream& operator<<(std::ostream&, const project_info&);


    class project
    {
    public:
        explicit project(const std::filesystem::path& file);

        project(const project&) = delete;
        project& operator=(const project&) = delete;

        project(project&&) = delete;
        project& operator=(project&&) = delete;

        [[nodiscard]] std::filesystem::path asset_directory() const;

        [[nodiscard]] std::filesystem::path cache_directory() const;

        [[nodiscard]] std::filesystem::path meta_directory() const;

        [[nodiscard]] uuid create_asset();

        void package_assets_as_single_bundle(const std::filesystem::path& where);

    public:
        /// @brief Database-like table of available assets.
        struct asset_table_t
        {
            /// @brief Assets uuids.
            std::vector<uuid> guids;

            /// @brief Assets names.
            std::vector<std::string> names;

            /// @brief Assets full paths on local disk.
            std::vector<std::filesystem::path> paths;

            /// @brief Assets binary size on local disk.
            std::vector<std::size_t> sizes;
        } assets;

    private:
        using _project_config = std::unordered_map<std::string, std::string>;

        /// @brief Root of project tree.
        std::filesystem::path _tree_root;

        _project_config _config;
    };


    /// @brief Internal asset descriptor.
    ///
    /// Holds information about an asset stored as engine-ready file.
    ///
    struct internal_asset_info
    {
        uuid        uuid;
        asset_type  type;
        std::string name;
        api_version version = build_api_version();
    };

    std::istream& load(std::istream&, internal_asset_info&);
    std::ostream& save(std::ostream&, const internal_asset_info&);


    /// @ brief External asset descriptor.
    ///
    /// Holds information about an asset stored as native file.
    ///
    struct external_asset_info
    {
        uuid                  uuid;
        std::filesystem::path path;
        api_version           version = build_api_version();
    };

    std::istream& load(std::istream&, external_asset_info&);
    std::ostream& save(std::ostream&, const external_asset_info&);


    struct asset_import_context
    {
        std::unordered_map<std::string, std::string> properties;
    };

    using asset_import_function = void (*)(const project_info&, const std::filesystem::path&, const asset_import_context&);
} // namespace drako::editor

#endif // !DRAKO_PROJECT_TYPES_HPP