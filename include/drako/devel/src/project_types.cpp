#include "drako/devel/project_types.hpp"

#include "drako/devel/asset_bundle_manifest.hpp"
#include "drako/file_formats/dson/dson.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iosfwd>

namespace drako::editor
{
    namespace _fs = std::filesystem;

    const dson::DOM& operator>>(const dson::DOM& is, ProjectMetaInfo& info)
    {
        from_string(is.get("version"), info.version);
        info.name   = is.get("name");
        //info.author = is.get("author");
        return is;
    }

    dson::DOM& operator<<(dson::DOM& os, const ProjectMetaInfo& info)
    {
        os.set("version", to_string(info.version));
        os.set("name", info.name);
        //os.set("author", info.author);
        return os;
    }


    // Retrive an asset metafile path from its guid.
    _fs::path ProjectContext::guid_to_metafile(const Uuid& id) const
    {
        return meta_directory() / (to_string(id) + ".dkmeta");
    }

    // Retrive an asset datafile path from its guid.
    _fs::path ProjectContext::guid_to_datafile(const Uuid& id) const
    {
        return cache_directory() / (to_string(id) + ".dkdata");
    }

    void package_assets_as_single_bundle(const ProjectDatabase& p, const _fs::path& where)
    {
        if (_fs::exists(where))
            throw std::invalid_argument{ "File already exists." };

        std::ofstream package{ where, std::ios_base::binary };
        package.exceptions(std::ios_base::failbit | std::ios_base::badbit);

        const auto max_size = std::max_element(
            p.assets.sizes.cbegin(), p.assets.sizes.cend());
        auto staging_buffer = std::make_unique<char[]>(*max_size);

        for (auto i = 0; i < std::size(p.assets.ids); ++i)
        {
            std::ifstream binary{ p.assets.paths[i], std::ios_base::binary };
            binary.exceptions(std::ios_base::failbit | std::ios_base::badbit);

            binary.read(staging_buffer.get(), p.assets.sizes[i]);
            package.write(staging_buffer.get(), p.assets.sizes[i]);
        }
    }

    /*
    std::istream& load(std::istream& is, internal_asset_info& inter)
    {
        dson::DOM serialized{};
        is >> serialized;
        parse(serialized.get("uuid"), inter.id);
        inter.name = serialized.get("name");
        from_string(serialized.get("version"), inter.version);
        return is;
    }

    std::ostream& save(std::ostream& os, const internal_asset_info& inter)
    {
        dson::DOM serialized{};
        serialized.set("uuid", to_string(inter.id));
        serialized.set("name", inter.name);
        serialized.set("version", to_string(inter.version));
        os << serialized;
        return os;
    }

    dson::DOM& operator>>(dson::DOM& is, internal_asset_info& inter)
    {
        parse(is.get("uuid"), inter.id);
        inter.name = is.get("name");
        from_string(is.get("version"), inter.version);
        return is;
    }

    dson::DOM& operator<<(dson::DOM& os, const internal_asset_info& inter)
    {
        os.set("uuid", to_string(inter.id));
        os.set("name", inter.name);
        os.set("version", to_string(inter.version));
        return os;
    }
    */

    std::istream& load(std::istream& is, AssetImportInfo& ext)
    {
        dson::DOM serialized{};
        is >> serialized;
        parse(serialized.get("uuid"), ext.id);
        ext.path = serialized.get("path");
        from_string(serialized.get("version"), ext.version);
        return is;
    }

    std::ostream& save(std::ostream& os, const AssetImportInfo& ext)
    {
        dson::DOM serialized{};
        serialized.set("uuid", to_string(ext.id));
        serialized.set("path", ext.path.string());
        serialized.set("version", to_string(ext.version));
        return os;
    }

    const dson::DOM& operator>>(const dson::DOM& is, AssetImportInfo& ext)
    {
        parse(is.get("uuid"), ext.id);
        ext.path = is.get("path");
        from_string(is.get("version"), ext.version);
        return is;
    }

    dson::DOM& operator<<(dson::DOM& os, const AssetImportInfo& ext)
    {
        os.set("uuid", to_string(ext.id));
        os.set("path", ext.path.string());
        os.set("version", to_string(ext.version));
        return os;
    }

} // namespace drako::editor