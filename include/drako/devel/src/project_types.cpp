#include "drako/devel/project_types.hpp"

#include "drako/devel/asset_bundle_manifest.hpp"
#include "drako/devel/mesh_importers.hpp"
//#include "drako/file_formats/dson/dson.hpp"

#include <yaml-cpp/yaml.h>

#include <exception>
#include <filesystem>
#include <fstream>
#include <iosfwd>

namespace drako::editor
{
    namespace _fs = std::filesystem;

    /*const dson::DOM& operator>>(const dson::DOM& dom, ProjectManifest& pm)
    {
        pm.editor_version = Version{ dom.get("editor_version") };
        pm.name           = dom.get("name");
        pm.root           = dom.get("root");
        pm.author         = dom.get("author");
        return dom;
    }

    dson::DOM& operator<<(dson::DOM& dom, const ProjectManifest& pm)
    {
        dom.set("editor_version", pm.editor_version.string());
        dom.set("name", pm.name);
        dom.set("root", pm.root.string());
        dom.set("author", pm.author);
        return dom;
    }*/

    const YAML::Node& operator>>(const YAML::Node& n, ProjectManifest& pm)
    {
        pm.editor_version = Version{ n["editor_version"].as<std::string>() };
        pm.name           = n["name"].as<std::string>();
        pm.root           = n["root"].as<std::string>();
        pm.author         = n["author"].as<std::string>();
        return n;
    }

    YAML::Node& operator<<(YAML::Node& n, const ProjectManifest& pm)
    {
        n["editor_version"] = pm.editor_version.string();
        n["name"]           = pm.name;
        n["root"]           = pm.root.string();
        n["author"]         = pm.author;
        return n;
    }


    ProjectContext::ProjectContext(const std::filesystem::path& root)
        : _root{ root }
    {
        if (!_fs::is_directory(_root))
            throw std::invalid_argument{ "project : root folder must be a valid directory" };

        //
        // Project tree structure:
        //
        // root
        // |- project_config.drako.proj
        // |- ./assets
        // |- ./cache
        // |- ./meta
        //
        _fs::create_directory(asset_directory());
        _fs::create_directory(cache_directory());
        _fs::create_directory(meta_directory());

        const ProjectManifest info{ .name = "new_drako_project" };
        _save_by_path(_root / "project.dkproj", info);
    }


    ProjectContext::ProjectContext(const ProjectManifest& pm)
        : _root{ pm.root }
    {
        if (!_fs::is_directory(_root))
            throw std::invalid_argument{ "project : root folder must be a valid directory" };

        //
        // Project tree structure:
        //
        // root
        // |- project_config.drako.proj
        // |- ./assets
        // |- ./cache
        // |- ./meta
        //
        _fs::create_directory(asset_directory());
        _fs::create_directory(cache_directory());
        _fs::create_directory(meta_directory());
    }




    // Retrive an asset metafile path from its guid.
    [[nodiscard]] _fs::path ProjectContext::guid_to_metafile(const uuid::Uuid& id) const
    {
        return meta_directory() / (to_string(id) + ".dkmeta");
    }

    // Retrive an asset datafile path from its guid.
    [[nodiscard]] _fs::path ProjectContext::guid_to_datafile(const uuid::Uuid& id) const
    {
        return cache_directory() / (to_string(id) + ".dkdata");
    }

    uuid::Uuid ProjectContext::import(const _fs::path& asset)
    {
        if (!_fs::exists(asset) || !_fs::is_regular_file(asset))
            throw std::runtime_error{ "editor : source is not a file." };

        const auto id             = generate_asset_uuid();
        const auto meta_file_path = guid_to_metafile(id);
        if (_fs::exists(meta_file_path))
            throw std::runtime_error{ "editor : conflict with already existing asset" };
        try
        {
            _save_by_path(meta_file_path, AssetImportInfo{ .id = id });
        }
        catch (...)
        { // clean up all the files that may have been generated
            _fs::remove(meta_file_path);
            throw;
        }
        return id;
    }

    void ProjectContext::compile(const AssetImportInfo& info)
    {
        const auto& ext = info.path.extension().string();
        if (ext == ".obj")
        {
            throw std::runtime_error{ "Cannot import .obj file" };
        }

        throw std::runtime_error{ "Unsupported file extension " + ext };
    }

    [[nodiscard]] ProjectContext::AssetScanResult ProjectContext::scan_all_assets() const
    {
        AssetScanResult sr{};
        for (const auto& f : _fs::directory_iterator{ meta_directory() })
        {
            try
            {
                if (_fs::is_regular_file(f) && f.path().extension() == ".dkmeta")
                {
                    AssetImportInfo info;
                    _load_by_path(f.path(), info);
                    sr.assets.push_back(info);
                }
            }
            catch (const _fs::filesystem_error& e)
            {
                //std::cout << "Error occurred while loading an asset metafile:\n"
                //          << '\t' << e.what() << '\n'
                //          << "\twith path1: " << e.path1() << '\n'
                //          << "\twith path2: " << e.path2() << '\n';
                sr.errors.push_back({ .asset = f.path(), .message = e.what() });
            }
        }
        return sr;
    }



    ProjectDatabase::ProjectDatabase(const ProjectContext& pc)
    {
        const auto scan = pc.scan_all_assets();
    }

    void ProjectDatabase::insert_asset(const _fs::path& src, const AssetImportInfo& info)
    {
        const auto size = static_cast<std::size_t>(_fs::file_size(src));

        _assets.ids.push_back(info.id);
        _assets.names.push_back(info.name);
        _assets.sizes.push_back(size);
        _assets.paths.push_back(src);
    }

    void ProjectDatabase::package_as_single_bundle(const _fs::path& where)
    {
        if (_fs::exists(where))
            throw std::invalid_argument{ "File already exists." };

        std::ofstream package{ where, std::ios_base::binary };
        package.exceptions(std::ios_base::failbit | std::ios_base::badbit);

        const auto max_size = std::max_element(
            _assets.sizes.cbegin(), _assets.sizes.cend());
        auto staging_buffer = std::make_unique<char[]>(*max_size);

        for (auto i = 0; i < std::size(_assets.ids); ++i)
        {
            std::ifstream binary{ _assets.paths[i], std::ios_base::binary };
            binary.exceptions(std::ios_base::failbit | std::ios_base::badbit);

            binary.read(staging_buffer.get(), _assets.sizes[i]);
            package.write(staging_buffer.get(), _assets.sizes[i]);
        }
    }

    /*const dson::DOM& operator>>(const dson::DOM& is, AssetImportInfo& ext)
    {
        ext.id      = uuid::Uuid{ is.get("uuid") };
        ext.path    = is.get("path");
        ext.name    = is.get("name");
        ext.version = Version{ is.get("version") };
        return is;
    }

    dson::DOM& operator<<(dson::DOM& os, const AssetImportInfo& a)
    {
        os.set("uuid", a.id.string());
        os.set("path", a.path.string());
        os.set("name", a.name);
        os.set("version", a.version.string());
        return os;
    }*/

    const YAML::Node& operator>>(const YAML::Node& n, AssetImportInfo& a)
    {
        a.id      = uuid::Uuid{ n["uuid"].as<std::string>() };
        a.path    = n["path"].as<std::string>();
        a.name    = n["name"].as<std::string>();
        a.version = Version{ n["version"].as<std::string>() };
        return n;
    }

    YAML::Node& operator<<(YAML::Node& n, const AssetImportInfo& a)
    {
        n["uuid"]    = a.id.string();
        n["path"]    = a.path.string();
        n["name"]    = a.name;
        n["version"] = a.version.string();
        return n;
    }

} // namespace drako::editor