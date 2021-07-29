#include "drako/devel/project_types.hpp"

#include "drako/devel/asset_bundle_types.hpp"
#include "drako/devel/mesh_importers.hpp"
#include "drako/devel/project_utils.hpp"

#include <uuid-cpp/uuid.hpp>
#include <yaml-cpp/yaml.h>

#include <exception>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <string>

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
        //pm.root           = n["root"].as<std::string>();
        pm.author = n["author"].as<std::string>();
        return n;
    }

    YAML::Node& operator<<(YAML::Node& n, const ProjectManifest& pm)
    {
        n["editor_version"] = pm.editor_version.string();
        n["name"]           = pm.name;
        //n["root"]           = pm.root.string();
        n["author"] = pm.author;
        return n;
    }


    ProjectContext::ProjectContext(const std::filesystem::path& root)
        : _root{ root }
    {
        if (!_fs::is_directory(_root))
            throw std::invalid_argument{ "project : root folder must be a valid directory" };

        _fs::create_directory(asset_data_directory(_root));
        _fs::create_directory(asset_meta_directory(_root));
        _fs::create_directory(bundle_data_directory(_root));
        _fs::create_directory(bundle_meta_directory(_root));
    }


    ProjectContext::ProjectContext(const _fs::path& root, const ProjectManifest& pm)
        : _root{ root }
    {
        if (!_fs::is_directory(_root))
            throw std::invalid_argument{ "project : root folder must be a valid directory" };

        _fs::create_directory(asset_data_directory(_root));
        _fs::create_directory(asset_meta_directory(_root));
        _fs::create_directory(bundle_data_directory(_root));
        _fs::create_directory(bundle_meta_directory(_root));

        save_text_format(_root / "project.yaml", pm);
    }




    // Retrive an asset metafile path from its guid.
    [[nodiscard]] _fs::path ProjectContext::guid_to_metafile(const AssetID& id) const
    {
        return asset_meta_directory(_root) / editor::guid_to_metafile(id);
    }

    // Retrive an asset datafile path from its guid.
    [[nodiscard]] _fs::path ProjectContext::guid_to_datafile(const AssetID& id) const
    {
        return asset_data_directory(_root) / editor::guid_to_datafile(id);
    }

    const YAML::Node& operator>>(const YAML::Node& n, AssetImportInfo& a)
    {
        a.guid    = uuid::parse(n["guid"].as<std::string>());
        a.path    = n["path"].as<std::string>();
        a.name    = n["name"].as<std::string>();
        a.version = Version{ n["version"].as<std::string>() };
        return n;
    }

    YAML::Node& operator<<(YAML::Node& n, const AssetImportInfo& a)
    {
        n["guid"]    = a.guid.string();
        n["path"]    = a.path.string();
        n["name"]    = a.name;
        n["version"] = a.version.string();
        return n;
    }

} // namespace drako::editor