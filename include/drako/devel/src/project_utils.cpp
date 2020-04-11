#pragma once
#include "drako/devel/project_utils.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/project_types.hpp"
#include "drako/devel/uuid.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace drako::editor
{

    struct _serialized_project_importable
    {
    };

    fs::path _external_asset_to_metafile(const fs::path& source);

    fs::path _guid_to_metafile(const uuid& guid) noexcept;


    [[nodiscard]] project_info load_project_info(const fs::path& metafile)
    {
        if (!fs::exists(metafile))
            throw std::invalid_argument{ DRAKO_STRINGIZE(metafile) };

        if (!fs::is_regular_file(metafile))
            throw std::invalid_argument{ DRAKO_STRINGIZE(metafile) };

        std::ifstream file{ metafile };
        project_info  info;

        file >> info;
        return info;
    }

    [[nodiscard]] internal_asset_info load_meta_info(const fs::path& metafile)
    {
        DRAKO_ASSERT(fs::is_regular_file(metafile));

        if (!fs::exists(metafile))
            throw std::invalid_argument{ DRAKO_STRINGIZE(metafile) };

        std::ifstream       file{ metafile };
        internal_asset_info info;
        file >> info;

        if (!file)
            return;

        return info;
    }

    void make_project_tree(const fs::path& root)
    {
        DRAKO_ASSERT(fs::is_directory(root));

        if (fs::exists(root))
            throw std::invalid_argument{ DRAKO_STRINGIZE(root) };

        try
        {
            fs::create_directories(root);
            fs::create_directory(root / PROJECT_ASSET_DIR);
            fs::create_directory(root / PROJECT_CACHE_DIR);
            fs::create_directory(root / PROJECT_META_DIR);
        }
        catch (const fs::filesystem_error&)
        {
            fs::remove_all(root);
            throw;
        }

        std::ofstream config{ root / PROJECT_CONFIG_FILE };
        if (config.good())
        {
            // TODO: write config file
            //config.write();
        }
    }

    void import_external_asset(const project_info& p, const fs::path& src)
    {
        if (!fs::exists(src) || !fs::is_regular_file(src))
            throw std::invalid_argument{ DRAKO_STRINGIZE(src) };

        try
        {
            const auto meta_file_path = _external_asset_to_metafile(src);
            if (fs::exists(meta_file_path))
                throw std::logic_error("Conflict with already existing asset");

            external_asset_info info{};

            std::ofstream meta_file{ meta_file_path };
        }
        catch (...)
        {
        }
    }

    void make_imported_asset(
        const project_info&          project,
        const std::vector<fs::path>& inputs,
        const std::vector<fs::path>& outputs)
    {
        DRAKO_ASSERT(!std::empty(inputs));
        DRAKO_ASSERT(!std::empty(outputs));

        const auto guid = make_uuid_version1();

        // TODO: end impl
    }

    [[nodiscard]] load_importable_result load_imported_asset(const uuid& guid)
    {
        DRAKO_ASSERT(guid.has_value(), "Invalid UUID");

        // TODO: end impl
    }

    void make_asset(const project_info& p, const fs::path& asset)
    {
        DRAKO_ASSERT(fs::is_regular_file(asset));

        const auto          guid = make_uuid_version1();
        internal_asset_info metainfo{};
        std::ofstream       metafile{ p.cache_directory() / to_string(guid) };

        metafile << metainfo;
    }

    [[nodiscard]] project load_project(const project_info& p)
    {
        // std::vector<uuid> uuids;
        // std::vector<internal_asset_info> assets;
        project result;

        /* load project assets metadata */
        for (const auto& entry : fs::directory_iterator{ p.meta_directory() })
        {
            try
            {
                const auto asset_info = load_meta_info(entry.path());
                result.asset_uuids.push_back(asset_info.guid);
                result.asset_names.push_back(asset_info.name);
                //result.asset_infos.push_back()
            }
            catch (const fs::filesystem_error& e)
            {
                std::cerr << "Error " << e.code() << " (" << e.path1() << "," << e.path2() << "):\n"
                          << e.what() << '\n';
                throw;
            }
        }
        return result;
    }

    [[nodiscard]] fs::path guid_to_path(const project_info& p, const uuid& asset)
    {
        return p.cache_directory() / to_string(asset);
    }

    // build_error build_devel_project(const project_info& p);

} // namespace drako::editor