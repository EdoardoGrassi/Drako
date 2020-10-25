#pragma once

#include "drako/devel/project_types.hpp"

#include <exception>
#include <filesystem>
#include <fstream>

namespace drako::editor
{
    project::project(const fs::path& file)
        : _tree_root(file.root_directory())
    {
        // TODO: load settings configuration from project file

        fs::create_directories(asset_directory());
        fs::create_directories(cache_directory());
        fs::create_directories(meta_directory());
    }

    [[nodiscard]] fs::path project::asset_directory() const
    {
        return _tree_root / "assets";
    }

    [[nodiscard]] fs::path project::cache_directory() const
    {
        return _tree_root / "cache";
    }

    [[nodiscard]] fs::path project::meta_directory() const
    {
        return _tree_root / "meta";
    }


    internal_asset_info::internal_asset_info(const fs::path& asset)
        : _guid(make_uuid_version1())
    {
    }

    [[nodiscard]] internal_asset_info from_stream(std::istream& is)
    {
        uuid guid;
        is >> guid;

        return internal_asset_info(guid);
    }

    std::ostream& to_stream(std::ostream& os, const internal_asset_info& info)
    {
        os << info._guid;
        return os;
    }

} // namespace drako::editor