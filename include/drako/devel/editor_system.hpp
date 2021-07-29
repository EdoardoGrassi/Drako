#pragma once
#ifndef DRAKO_EDITOR_SYSTEM_HPP
#define DRAKO_EDITOR_SYSTEM_HPP

#include "drako/devel/project_types.hpp"
#include "drako/devel/project_utils.hpp"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <span>
#include <vector>

namespace drako::editor
{
    class EditorError : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };


    /// @brief Editor instance associated to a project.
    class EditorSystem
    {
    public:
        using ImportErrorHandler = std::function<void(const AssetImportError&)>;


        explicit EditorSystem(
            const ProjectContext&                     context,
            const AssetImportStack&                   stack,
            const std::span<const ImportErrorHandler> error_handlers);


        /// @brief Add a new asset to the project.
        /// @param name Asset name
        /// @param file
        void create_asset(
            const std::string_view name, const std::filesystem::path& file);

        void destroy_asset();

        // TODO: add ability to attach/detach error handlers

        //void attach_handler(const );
        //void detach_handler();

    private:
        ProjectContext                  _context;
        ProjectDatabase                 _database;
        AssetImportStack                _importers;
        std::vector<ImportErrorHandler> _error_handlers;

        // insert an asset in the database
        void _insert_asset(const AssetImportInfo& a)
        {
            _database.assets.guids.emplace_back(a.guid);
            _database.assets.paths.emplace_back(a.path);
            _database.assets.names.emplace_back(a.name);
        }

        // chech whether an asset name is unique inside the project
        [[nodiscard]] bool _unique_asset_name(const std::string_view name) const
        {
            const auto& t = _database.assets.names;
            return std::find(std::cbegin(t), std::cend(t), name) == std::cend(t);
        }
    };




} // namespace drako::editor

#endif // !DRAKO_EDITOR_SYSTEM_HPP