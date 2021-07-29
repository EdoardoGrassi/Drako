#include "drako/devel/editor_system.hpp"

#include "drako/devel/project_utils.hpp"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace drako::editor
{
    EditorSystem::EditorSystem(
        const ProjectContext&                     context,
        const AssetImportStack&                   stack,
        const std::span<const ImportErrorHandler> error_handlers)
        : _context{ context }
        , _importers{ stack }
        , _error_handlers{ std::cbegin(error_handlers), std::cend(error_handlers) }
    {
        const auto scan = scan_all_assets(_context);
        _database.assets.guids.reserve(std::size(scan.assets));
        _database.assets.names.reserve(std::size(scan.assets));
        _database.assets.paths.reserve(std::size(scan.assets));

        /* for (const auto& a : scan.assets)
            {
                _database.assets.guids.emplace_back(std::move(a.guid));
                _database.assets.names.emplace_back(std::move(a.name));
                _database.assets.paths.emplace_back(std::move(a.path));
            }*/
        for (const auto& a : scan.assets)
            _insert_asset(a);

        for (const auto& h : _error_handlers)
            for (const auto& e : scan.errors)
                std::invoke(h, e);
    }

    void EditorSystem::create_asset(
        const std::string_view       name,
        const std::filesystem::path& file)
    {
        if (empty(name))
            throw std::invalid_argument{ "empty name not allowed" };
        if (!fs::exists(file))
            throw std::runtime_error{ "asset file does not exist" };
        if (!_unique_asset_name(name))
            throw std::runtime_error{ "asset name already in use" };

        const auto&           loader = _importers.at(file.extension().string());
        const AssetImportInfo info{
            .guid = uuid::SystemEngine{}(),
            .path = file,
            .name = std::string{ name },
        };
        try
        {
            create_asset_meta(_context, info);
            import_asset_data(_context, loader, info);
        }
        catch (const fs::filesystem_error& e)
        {
            destroy_asset_meta(_context, info);
            destroy_asset_data(_context, info);
            throw;
        }
        _insert_asset(info);
    }
} // namespace drako::editor