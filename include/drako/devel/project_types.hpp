#pragma once
#ifndef DRAKO_PROJECT_TYPES_HPP_
#define DRAKO_PROJECT_TYPES_HPP_

#include "drako/core/drako_api_defs.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/uuid.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>


namespace fs = std::filesystem; // convenient alias

namespace drako::editor
{

    //
    // Subdirectory of project tree.
    // Contains the assets available to the project.
    //
    const fs::path PROJECT_ASSET_DIR{ "./assets" };

    //
    // Subdirectory of project tree.
    // Contains a cache of imported and preprocessed assets.
    //
    const fs::path PROJECT_CACHE_DIR{ "./cache" };

    //
    // Subdirectory of project tree.
    // Contains metafiles associated to assets.
    //
    const fs::path PROJECT_META_DIR{ "./meta" };

    //
    // Main project file located at the root of the project tree.
    //
    const fs::path PROJECT_CONFIG_FILE{ "project_config.drako.proj" };

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
        std::string name;
        //_fs::file_time_type last_project_scan;
        fs::path root;

        [[nodiscard]] fs::path asset_directory() const { return root / PROJECT_ASSET_DIR; }

        [[nodiscard]] fs::path cache_directory() const { return root / PROJECT_CACHE_DIR; }

        [[nodiscard]] fs::path meta_directory() const { return root / PROJECT_META_DIR; }
    };

    std::istream& operator>>(std::istream& is, project_info& p)
    {
        is >> p.name;
        //is.get();
        //is >> p.last_project_scan.time_since_epoch().count();
        //is.get();
        //is >> p.root;
    }

    std::ostream& operator<<(std::ostream& os, const project_info& p)
    {
        os << DRAKO_STRINGIZE(name) ": " << p.name << '\n';
        os << DRAKO_STRINGIZE(last_project_scan) ": " << p.last_project_scan.time_since_epoch().count() << '\n';
        os << DRAKO_STRINGIZE(root) ": " << p.root << '\n';
        return os;
    }


    //
    // @brief Internal asset descriptor.
    //
    // Holds information about an asset stored as engine-ready file.
    //
    struct internal_asset_info
    {
        uuid          guid;
        std::uint32_t version;
        std::uint32_t crc;
        std::uint32_t size_bytes;
        asset_type    type;
        std::string   name;
    };

    std::istream& operator>>(std::istream& is, internal_asset_info& p)
    {
        return is >> p.guid >> p.version >> p.crc;
    }

    std::ostream& operator<<(std::ostream& os, const internal_asset_info& p)
    {
        return os << p.guid << '\n'
                  << p.version << '\n'
                  << p.crc;
    }


    //
    // @ brief External asset descriptor.
    //
    // Holds information about an asset stored as native file.
    //
    class external_asset_info
    {
    public:
        explicit external_asset_info() noexcept
            : _version(build_api_version())
            , _guid(make_uuid_version1())
        {
        }

        external_asset_info(const external_asset_info&) = delete;
        external_asset_info& operator=(const external_asset_info&) = delete;

    private:
        api_version _version;
        uuid        _guid;
    };


    struct asset_import_context
    {
        std::unordered_map<std::string, std::string> options;
    };

    using asset_import_function = void (*)(const project_info&, const fs::path&, const asset_import_context&);
} // namespace drako::editor

#endif // !DRAKO_PROJECT_TYPES_HPP_