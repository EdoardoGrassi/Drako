#include "drako/devel/project_utils.hpp"

#include "drako/devel/project_types.hpp"

#include <uuid-cpp/uuid.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace _fs = std::filesystem;

namespace drako::editor
{
    void create_asset_meta(const ProjectContext& pc, const AssetImportInfo& i)
    {
        save_text_format(asset_meta_path(pc, i.guid), i);
    }

    void destroy_asset_meta(const ProjectContext& pc, const AssetImportInfo& i)
    {
        _fs::remove(asset_meta_path(pc, i.guid));
    }

    void destroy_asset_data(const ProjectContext& pc, const AssetImportInfo& i)
    {
        _fs::remove(asset_data_path(pc, i.guid));
    }

    void import_asset_data(
        const ProjectContext& c, const AssetImportFunction& f, const AssetImportInfo& i)
    {
        if (!_fs::exists(i.path))
            throw std::runtime_error{ "asset file does not exist" };

        std::invoke(f, c, i, AssetImportContext{});
    }


    [[nodiscard]] AssetScanResult scan_all_assets(const ProjectContext& pc)
    {
        AssetScanResult sr{};
        for (const auto& f : _fs::directory_iterator{ asset_meta_directory(pc) })
        {
            try
            {
                if (_fs::is_regular_file(f))
                {
                    AssetImportInfo info;
                    load_text_format(f.path(), info);
                    sr.assets.push_back(info);
                }
            }
            catch (const _fs::filesystem_error& e)
            {
                sr.errors.push_back({ .file = f.path(), .message = e.what() });
            }
        }
        return sr;
    }

    [[nodiscard]] BundleScanResult scan_all_bundles(const ProjectContext& pc)
    {
        BundleScanResult sr{};
        for (const auto& f : _fs::directory_iterator{ bundle_meta_directory(pc) })
        {
            try
            {
                if (_fs::is_regular_file(f))
                {
                    AssetBundleInfo info;
                    load_text_format(f.path(), info);
                    sr.bundles.push_back(info);
                }
            }
            catch (const _fs::filesystem_error& e)
            {
                sr.errors.push_back({ .file = f.path(), .message = e.what() });
            }
        }
        return sr;
    }

    AssetBundleManifest package_as_single(
        const ProjectContext& pc, const ProjectDatabase& pd)
    {
        AssetBundleManifest a;
        a.header.name = "__main";
        a.header.uuid = uuid::SystemEngine{}();
        a.guids       = pd.assets.guids;
        return a;
    }

} // namespace drako::editor