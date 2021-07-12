#include "drako/devel/project_utils.hpp"

#include "drako/devel/project_types.hpp"
#include "drako/file_formats/dson/dson.hpp"

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

    [[nodiscard]] _fs::path _external_asset_to_metafile(const _fs::path& source)
    {
        return _fs::path{ source }.append(".dkmeta");
    }

    void _create_project_info_file(const _fs::path& where);

    /*
    void load_asset_index_cache(const ProjectDatabase& p)
    {
        std::ifstream f{ p.cache_directory() / "asset_index" };
        f.exceptions(std::ios_base::failbit | std::ios_base::badbit);

        while (f)
        {
            _fs::path path;
            f >> path;
        }
    }
    */

    
    /* void create_project_tree(const ProjectContext& ctx, std::string_view name)
    {
        assert(!std::empty(name));

        if (!_fs::exists(ctx.root().parent_path()))
            throw std::runtime_error{ "editor : parent directory does not exists." };
        if (!_fs::create_directory(ctx.root()))
            throw std::runtime_error{ "editor : project directory already exists." };

        try
        {
            _fs::create_directory(ctx.asset_directory());
            _fs::create_directory(ctx.cache_directory());
            _fs::create_directory(ctx.meta_directory());

            const ProjectManifest info{
                .name = std::string{ name }
            };
            save(ctx.root() / "project.dkproj", info);
        }
        catch (const _fs::filesystem_error&)
        {
            _fs::remove_all(ctx.root());
            throw;
        }
    }

    void create_asset(const ProjectContext& ctx, ProjectDatabase& db, const _fs::path& p, const AssetImportInfo& info)
    {
        const auto guid           = ctx.generate_asset_uuid();
        const auto meta_file_path = ctx.guid_to_metafile(guid);
        try
        {
            // generate asset metafile on disk
            save(meta_file_path, info);

            // add asset in memory database
            db.insert_asset(p, info);
        }
        catch (const std::system_error& e)
        {
            std::cout << "Asset " << p << " creation failed: " << e.what() << '\n';
            std::filesystem::remove(meta_file_path);
            throw;
        }
    } */


    /* void load_all_assets(const ProjectContext& ctx, ProjectDatabase& db)
    {
        // scan the whole meta folder for metafiles
        for (const auto& f : _fs::directory_iterator{ ctx.meta_directory() })
        {
            try
            {
                if (_fs::is_regular_file(f) && f.path().extension() == ".dkmeta")
                {
                    AssetImportInfo info;
                    load(f, info);

                    _fs::path asset_file{ f.path() };
                    asset_file.replace_extension(); // remove .dkmeta
                    insert(db, asset_file, info);
                }
            }
            catch (const _fs::filesystem_error& e)
            {
                std::cout << "Error occurred while loading an asset metafile:\n"
                          << '\t' << e.what() << '\n'
                          << "\twith path1: " << e.path1() << '\n'
                          << "\twith path2: " << e.path2() << '\n';
            }
        }
    }*/

    // build_error build_devel_project(const ProjectManifest& p);

} // namespace drako::editor