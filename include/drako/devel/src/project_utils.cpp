#include "drako/devel/project_utils.hpp"

#include "drako/devel/project_types.hpp"
#include "drako/devel/uuid.hpp"
#include "drako/devel/uuid_engine.hpp"
#include "drako/file_formats/dson/dson.hpp"
#include "drako/io/input_file_handle.hpp"
#include "drako/io/output_file_handle.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
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

    void insert(ProjectDatabase& p, const _fs::path& src, const AssetImportInfo& info)
    {
        const auto size = static_cast<std::size_t>(_fs::file_size(src));

        p.assets.ids.push_back(info.id);
        p.assets.names.push_back(info.name);
        p.assets.sizes.push_back(size);
        p.assets.paths.push_back(src);
    }

    void load(const _fs::path& file, ProjectMetaInfo& info)
    {
        if (!_fs::exists(file))
            throw std::invalid_argument{ DRAKO_STRINGIZE(file) };

        if (!_fs::is_regular_file(file))
            throw std::invalid_argument{ DRAKO_STRINGIZE(file) };

        const auto size = static_cast<std::size_t>(_fs::file_size(file));
        auto       buff = std::make_unique<char[]>(size + 1);

        std::ifstream{ file }.read(buff.get(), size);
        buff[size]     = '\0'; // set input stream termination
        const auto dom = dson::parse({ buff.get(), size + 1 });
        dom >> info;
    }

    void save(const _fs::path& p, const AssetImportInfo& info)
    {
        // serialize as dson object
        dson::DOM dom{};
        dom << info;
        const auto s = to_string(dom);

        io::UniqueOutputFile f{ p, io::Create::if_needed };
        io::write_all(f, { reinterpret_cast<const std::byte*>(std::data(s)), std::size(s) });
    }

    void load(const _fs::path& p, AssetImportInfo& info)
    {
        // TODO: remove narrowing conversion, check if the file is too big
        const auto size = static_cast<std::size_t>(_fs::file_size(p));
        auto       buff = std::make_unique<char[]>(size + 1);

        io::UniqueInputFile f{ p };
        io::read_exact(f, { reinterpret_cast<std::byte*>(buff.get()), size });
        buff[size] = '\0';

        const auto dom = dson::parse({ buff.get(), size });
        dom >> info;
    }

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

    void create_project_tree(const ProjectContext& ctx, std::string_view name)
    {
        assert(!std::empty(name));

        if (!_fs::create_directory(ctx.root()))
            throw std::runtime_error{ "Root directory already exists." };

        try
        {
            _fs::create_directory(ctx.asset_directory());
            _fs::create_directory(ctx.cache_directory());
            _fs::create_directory(ctx.meta_directory());

            const ProjectMetaInfo info{
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

    void import_asset(const ProjectContext& ctx, const _fs::path& src)
    {
        if (!_fs::exists(src) || !_fs::is_regular_file(src))
            throw std::runtime_error{ "Source isn't a file." };

        const auto id = ctx.generate_asset_uuid();

        const auto meta_file_path = _external_asset_to_metafile(src);
        if (_fs::exists(meta_file_path))
            throw std::runtime_error("Conflict with already existing asset");
        try
        {
            const AssetImportInfo info{ .id = id };
            dson::DOM             dom{};
            dom << info;

            std::ofstream meta_file{ meta_file_path };
            meta_file << dom;
        }
        catch (...)
        { // clean up all the files that may have been generated
            _fs::remove(meta_file_path);
            throw;
        }
    }

    void create_asset(
        const ProjectContext& ctx, ProjectDatabase& db, const _fs::path& p, const AssetImportInfo& info)
    {
        const auto guid           = ctx.generate_asset_uuid();
        const auto meta_file_path = ctx.guid_to_metafile(guid);
        try
        {
            // generate asset metafile on disk
            save(meta_file_path, info);

            // add asset in memory database
            insert(db, p, info);
        }
        catch (const std::system_error& e)
        {
            std::cout << "Asset " << p << " creation failed: " << e.what() << '\n';
            std::filesystem::remove(meta_file_path);
            throw;
        }
    }

    void load(const ProjectContext& ctx, ProjectDatabase& db)
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
        std::cout << "Project loaded!\n";
    }

    void save(const ProjectContext& ctx, const ProjectDatabase& p)
    {
        throw std::runtime_error{ "Not implemented yet!" };
    }

    // build_error build_devel_project(const ProjectMetaInfo& p);

} // namespace drako::editor