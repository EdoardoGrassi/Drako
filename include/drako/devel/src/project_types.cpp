#include "drako/devel/project_types.hpp"

#include "drako/devel/asset_bundle_manifest.hpp"
#include "drako/file_formats/dson.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iosfwd>

namespace drako::editor
{
    namespace _fs = std::filesystem;

    std::istream& operator>>(std::istream& is, project_info& info)
    {
        file_formats::dson::object serialized{};
        info.version = api_version::from_string(serialized.get("version"));
        info.name    = serialized.get("name");
        info.author  = serialized.get("author");
        return is;
    }

    std::ostream& operator<<(std::ostream& os, const project_info& info)
    {
        file_formats::dson::object serialized{};
        serialized.set("version", to_string(info.version));
        serialized.set("name", info.name);
        serialized.set("author", info.author);
        os << serialized;
        return os;
    }

    project::project(const _fs::path& file)
        : _tree_root(file.root_directory())
    {
        // TODO: load settings configuration from project file

        _fs::create_directories(asset_directory());
        _fs::create_directories(cache_directory());
        _fs::create_directories(meta_directory());
    }

    [[nodiscard]] _fs::path project::asset_directory() const
    {
        return _tree_root / "assets";
    }

    [[nodiscard]] _fs::path project::cache_directory() const
    {
        return _tree_root / "cache";
    }

    [[nodiscard]] _fs::path project::meta_directory() const
    {
        return _tree_root / "meta";
    }

    void project::package_assets_as_single_bundle(const _fs::path& where)
    {
        if (_fs::exists(where))
            throw std::invalid_argument{ "File already exists." };

        std::ofstream package{ where, std::ios_base::binary };
        package.exceptions(std::ios_base::failbit | std::ios_base::badbit);

        const auto max_size       = std::max_element(assets.sizes.cbegin(), assets.sizes.cend());
        auto       staging_buffer = std::make_unique<char[]>(*max_size);

        for (auto i = 0; i < std::size(assets.guids); ++i)
        {
            std::ifstream binary{ assets.paths[i], std::ios_base::binary };
            binary.exceptions(std::ios_base::failbit | std::ios_base::badbit);

            binary.read(staging_buffer.get(), assets.sizes[i]);
            package.write(staging_buffer.get(), assets.sizes[i]);
        }
    }


    std::istream& load(std::istream& is, internal_asset_info& inter)
    {
        drako::file_formats::dson::object serialized{};
        is >> serialized;
        inter.uuid    = uuid::parse(serialized.get("uuid"));
        inter.name    = serialized.get("name");
        inter.version = api_version::from_string(serialized.get("version"));
        return is;
    }

    std::ostream& save(std::ostream& os, const internal_asset_info& inter)
    {
        drako::file_formats::dson::object serialized{};
        serialized.set("uuid", to_string(inter.uuid));
        serialized.set("name", inter.name);
        serialized.set("version", to_string(inter.version));
        os << serialized;
        return os;
    }

    std::istream& load(std::istream& is, external_asset_info& ext)
    {
        drako::file_formats::dson::object serialized{};
        is >> serialized;
        ext.uuid    = uuid::parse(serialized.get("uuid"));
        ext.path    = serialized.get("path");
        ext.version = api_version::from_string(serialized.get("version"));
        return is;
    }

    std::ostream& save(std::ostream& os, const external_asset_info& ext)
    {
        drako::file_formats::dson::object serialized{};
        serialized.set("uuid", to_string(ext.uuid));
        serialized.set("path", ext.path.string());
        serialized.set("version", to_string(ext.version));
        return os;
    }

} // namespace drako::editor