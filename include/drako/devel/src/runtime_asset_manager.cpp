#pragma once
#include "drako/devel/runtime_asset_manager.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

namespace drako::engine
{
    runtime_asset_manager::runtime_asset_manager(
        const std::vector<guid<asset_bundle>>& bundle_guids,
        const std::vector<std::string>&        bundle_names,
        const std::vector<_fs::path>&          bundle_paths)
        : _available_bundles_guids(bundle_guids)
        , _available_bundles_names(bundle_names)
        , _available_bundles_sources(std::size(bundle_paths))
    {
        DRAKO_ASSERT(!std::empty(bundle_guids));
        DRAKO_ASSERT(!std::empty(bundle_names));
        DRAKO_ASSERT(!std::empty(bundle_paths));

        DRAKO_ASSERT(std::size(bundle_guids) == std::size(bundle_names));
        DRAKO_ASSERT(std::size(bundle_guids) == std::size(bundle_paths));

        for (const auto& file_path : bundle_paths)
            _available_bundles_sources.emplace_back(file_path, std::ios::binary);
    }

    [[nodiscard]] const asset_bundle&
    runtime_asset_manager::load_asset_bundle(const asset_bundle_id id) noexcept
    {
        // search between loaded bundles
        if (const auto found = std::find(
                std::cbegin(_loaded_bundles_guids), std::cend(_loaded_bundles_guids), id);
            found != std::cend(_loaded_bundles_guids))
        { // bundle already loaded, just increment reference count
            const auto index = std::distance(std::cbegin(_loaded_bundles_guids), found);
            ++_loaded_bundles_refcount[index];
            return;
        }

        // retrieve bundle metadata
        if (const auto found = std::find(
                std::cbegin(_available_bundles_guids), std::cend(_available_bundles_guids), id);
            found != std::cend(_available_bundles_guids))
        {
            const auto index = std::distance(std::cbegin(_available_bundles_guids), found);
        }
        else
        {
            DRAKO_LOG_ERROR("Can't locate asset bundle with id " + std::to_string(id));
            std::exit(EXIT_FAILURE);
        }
    }

    /*
    [[nodiscard]] const asset_bundle&
    runtime_asset_manager::load_asset_bundle(const std::string_view name) noexcept
    {
        // search between loaded bundles
        if (const auto found = std::find(
                std::cbegin(_loaded_bundles_guids), std::cend(_loaded_bundles_guids), id);
            found != std::cend(_loaded_bundles_guids))
        { // bundle already loaded
            return;
        }

        // retrieve bundle metadata
        if (const auto found = std::find(
                std::cbegin(_available_bundles_names), std::cend(_available_bundles_names), name);
            found != std::cend(_available_bundles_names))
        {
            const auto index = std::distance(std::cbegin(_available_bundles_names), found);
        }
        else
        {
            DRAKO_LOG_ERROR("Can't locate asset bundle with name " + std::string(name));
            std::exit(EXIT_FAILURE);
        }
    }
    */

    void runtime_asset_manager::load_asset(const std::vector<uuid>& ids) noexcept
    {
        for (const auto& id : ids)
            load_asset(id);
    }

    void runtime_asset_manager::unload_asset_bundle(asset_bundle_id id)
    {
        if (const auto bundle = std::find(
                std::cbegin(_loaded_bundles_guids), std::cend(_loaded_bundles_guids), id);
            bundle != std::cend(_loaded_bundles_guids))
        {
            const auto index = std::distance(std::cbegin(_loaded_bundles_guids), bundle);
            --_loaded_bundles_refcount[index];
            if (_loaded_bundles_refcount[index] == 0)
            {
                _loaded_bundles_guids.erase([index]);
                _loaded_bundles.erase([index]);
                _loaded_bundles_refcount.erase([index]);
            }
        }
        else
        {
            DRAKO_LOG_ERROR("No active instances of bundle with id " + std::to_string(id));
            std::exit(EXIT_FAILURE);
        }
    }

} // namespace drako::engine