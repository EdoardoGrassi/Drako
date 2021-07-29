#include "drako/devel/asset_bundle_types.hpp"

#include "drako/core/byte_stream.hpp"

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>

namespace drako
{
    const YAML::Node& operator>>(const YAML::Node& n, AssetBundleInfo& a)
    {
        a.name = n["name"].as<std::string>();
        a.guid = uuid::parse(n["guid"].as<std::string>());
        return n;
    }

    YAML::Node& operator<<(YAML::Node& n, const AssetBundleInfo& a)
    {
        n["name"] = a.name;
        n["guid"] = a.guid.string();
        return n;
    }


    std::istream& operator>>(std::istream& is, AssetBundleManifest& a)
    {
        throw std::runtime_error{ "Not implemented" };
    }

    std::ostream& operator<<(std::ostream& os, const AssetBundleManifest& a)
    {
        throw std::runtime_error{ "Not implemented" };
    }


    const YAML::Node& operator>>(const YAML::Node& n, AssetBundleManifest& a)
    {
        a.header.name    = n["name"].as<std::string>();
        a.header.uuid    = uuid::parse(n["uuid"].as<std::string>());
        a.header.version = Version{ n["version"].as<std::string>() };
        return n;
    }

    YAML::Node& operator<<(YAML::Node& n, const AssetBundleManifest& a)
    {
        //assert(std::size(a.ids) == std::size(a.infos));

        n["name"]    = a.header.name;
        n["uuid"]    = a.header.uuid.string();
        n["version"] = a.header.version.string();

        for (auto i = 0; i < std::size(a.guids); ++i)
        {
            n["assets"][i]["guid"] = a.guids[i].string();
        }
        return n;
    }

} // namespace drako