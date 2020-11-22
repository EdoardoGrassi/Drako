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

    dson::object& operator>>(dson::object& is, project_info& info)
    {
        from_string(is.get("version"), info.version);
        info.name   = is.get("name");
        info.author = is.get("author");
        return is;
    }

    dson::object& operator<<(dson::object& os, const project_info& info)
    {
        os.set("version", to_string(info.version));
        os.set("name", info.name);
        os.set("author", info.author);
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
        dson::object serialized{};
        is >> serialized;
        inter.uuid = uuid::parse(serialized.get("uuid"));
        inter.name = serialized.get("name");
        from_string(serialized.get("version"), inter.version);
        return is;
    }

    std::ostream& save(std::ostream& os, const internal_asset_info& inter)
    {
        dson::object serialized{};
        serialized.set("uuid", to_string(inter.uuid));
        serialized.set("name", inter.name);
        serialized.set("version", to_string(inter.version));
        os << serialized;
        return os;
    }

    dson::object& operator>>(dson::object& is, internal_asset_info& inter)
    {
        inter.uuid = uuid::parse(is.get("uuid"));
        inter.name = is.get("name");
        from_string(is.get("version"), inter.version);
        return is;
    }

    dson::object& operator<<(dson::object& os, const internal_asset_info& inter)
    {
        os.set("uuid", to_string(inter.uuid));
        os.set("name", inter.name);
        os.set("version", to_string(inter.version));
        return os;
    }


    std::istream& load(std::istream& is, external_asset_info& ext)
    {
        dson::object serialized{};
        is >> serialized;
        ext.uuid = uuid::parse(serialized.get("uuid"));
        ext.path = serialized.get("path");
        from_string(serialized.get("version"), ext.version);
        return is;
    }

    std::ostream& save(std::ostream& os, const external_asset_info& ext)
    {
        dson::object serialized{};
        serialized.set("uuid", to_string(ext.uuid));
        serialized.set("path", ext.path.string());
        serialized.set("version", to_string(ext.version));
        return os;
    }

    dson::object& operator>>(dson::object& is, external_asset_info& ext)
    {
        ext.uuid = uuid::parse(is.get("uuid"));
        ext.path = is.get("path");
        from_string(is.get("version"), ext.version);
        return is;
    }

    dson::object& operator<<(dson::object& os, const external_asset_info& ext)
    {
        os.set("uuid", to_string(ext.uuid));
        os.set("path", ext.path.string());
        os.set("version", to_string(ext.version));
        return os;
    }

} // namespace drako::editor