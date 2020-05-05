#pragma once
#include "drako/devel/runtime_asset_manager.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

namespace drako::engine
{
    [[nodiscard]] std::tuple<bool, std::size_t>
    runtime_asset_manager::_find_loaded_asset_index(const asset_id id) noexcept
    {
        if (const auto found = std::find(
                std::cbegin(_loaded_asset_guids), std::cend(_loaded_asset_guids), id);
            found != std::cend(_loaded_asset_guids))
        {
            return { true, std::distance(std::cbegin(_loaded_asset_guids), found) };
        }
        return { false, std::numeric_limits<std::size_t>::max() };
    }

    [[nodiscard]] std::tuple<bool, std::size_t>
    runtime_asset_manager::_find_loaded_bundle_index(const asset_bundle_id b) noexcept
    {
        if (const auto found = std::find(
                std::cbegin(_loaded_bundles_guids), std::cend(_loaded_bundles_guids), b);
            found != std::cend(_loaded_bundles_guids))
        {
            return { true, std::distance(std::cbegin(_loaded_bundles_guids), found) };
        }
        return { false, std::numeric_limits<std::size_t>::max() };
    }

    [[nodiscard]] std::vector<std::byte>
    runtime_asset_manager::_load_raw_asset() noexcept;

    void runtime_asset_manager::_execute_pending_bundle_requests() noexcept;


    runtime_asset_manager::runtime_asset_manager(
        const std::vector<asset_bundle_id>& bundle_guids,
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
    runtime_asset_manager::load_bundle(const asset_bundle_id id) noexcept
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

    void runtime_asset_manager::load_bundle_async(const asset_bundle_id id) noexcept
    {
        _bundle_load_request request{};
        _bundle_load_requests.push_back(request);
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

    void runtime_asset_manager::unload_bundle(asset_bundle_id id)
    {
        if (const auto [found, index] = _find_loaded_bundle_index(id); found)
        {
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