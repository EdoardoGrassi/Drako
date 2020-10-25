#pragma once
#ifndef DRAKO_PROJECT_TYPES_HPP
#define DRAKO_PROJECT_TYPES_HPP

#include "drako/core/drako_api_defs.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/uuid.hpp"

#include "drako/file_formats/drakoson.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>


namespace fs = std::filesystem; // convenient alias

namespace drako::editor
{
    const fs::path project_asset_dir{ "/assets" };
    const fs::path project_cache_dir{ "/cache" };
    const fs::path project_meta_dir{ "/meta" };

    //
    // Main project file located at the root of the project tree.
    //
    const fs::path project_config_file = "project_config.drako.proj";

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
        api_version version;
        std::string name;
        std::string author;
        //_fs::file_time_type last_project_scan;
    };



    void load_project_info(std::istream& is, project_info& info)
    {
        auto obj     = file_formats::drakoson::from_stream(is);
        info.version = from_string(obj["version"]);
        info.name    = obj["name"];
        info.author  = obj["author"];
    }

    void save_project_info(std::ostream& os, const project_info& info)
    {
        file_formats::drakoson::object obj{};
        obj["version"] = to_string(info.version);
        obj["name"]    = info.name;
        obj["author"]  = info.author;
        file_formats::drakoson::to_stream(obj, os);
    }


    class project
    {
    public:
        explicit project(const fs::path& file);

        project(const project&) = delete;
        project& operator=(const project&) = delete;

        project(project&&) = delete;
        project& operator=(project&&) = delete;

        [[nodiscard]] fs::path asset_directory() const;

        [[nodiscard]] fs::path cache_directory() const;

        [[nodiscard]] fs::path meta_directory() const;

        [[nodiscard]] uuid create_asset();

    public:
        struct asset_table_t
        {
            /// @brief Table column of vailable assets uuids.
            std::vector<uuid> asset_guids;

            /// @brief Table column of available assets names.
            std::vector<std::string> asset_names;

            /// @brief Table column of available assets full paths.
            std::vector<fs::path> asset_paths;
        };

        asset_table_t asset_table;


        // @brief Available assets info.
        //std::vector<std::uint32_t> asset_sizes;

    private:
        using _project_config = std::unordered_map<std::string, std::string>;

        /// @brief Root of project tree.
        fs::path _tree_root;

        _project_config _config;
    };


    /// @brief Internal asset descriptor.
    ///
    /// Holds information about an asset stored as engine-ready file.
    ///
    class internal_asset_info
    {
    public:
        explicit internal_asset_info(const fs::path& asset);

        explicit internal_asset_info(const uuid& guid);

        internal_asset_info(const internal_asset_info&) = delete;
        internal_asset_info& operator=(const internal_asset_info&) = delete;

        internal_asset_info(internal_asset_info&&) = delete;
        internal_asset_info& operator=(internal_asset_info&&) = delete;


        friend internal_asset_info from_stream(std::istream&);
        friend std::ostream&       to_stream(std::ostream&, const internal_asset_info&);

        [[nodiscard]] uuid guid() const noexcept;

        [[nodiscard]] std::string name() const noexcept;

        [[nodiscard]] fs::path path() const noexcept;

    private:
        const uuid  _guid;
        asset_type  _type;
        std::string _name;
    };


    /// @ brief External asset descriptor.
    ///
    /// Holds information about an asset stored as native file.
    ///
    class external_asset_info
    {
    public:
        explicit external_asset_info(const fs::path& asset)
            : _version(build_api_version())
            , _guid(make_uuid_version1())
            , _asset(asset)
        {
        }

        external_asset_info(const external_asset_info&) = delete;
        external_asset_info& operator=(const external_asset_info&) = delete;

        external_asset_info(external_asset_info&&) = delete;
        external_asset_info& operator=(external_asset_info&&) = delete;

    private:
        api_version _version;
        const uuid  _guid;
        fs::path    _asset;
    };


    struct asset_import_context
    {
        std::unordered_map<std::string, std::string> properties;
    };

    using asset_import_function = void (*)(const project_info&, const fs::path&, const asset_import_context&);
} // namespace drako::editor

#endif // !DRAKO_PROJECT_TYPES_HPP