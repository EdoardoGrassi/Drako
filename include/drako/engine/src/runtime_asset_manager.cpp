#include "drako/engine/runtime_asset_manager.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

namespace drako::engine
{
    [[noreturn]] void _log_and_exit(const std::string& msg)
    {
        DRAKO_LOG_ERROR(msg);
        std::exit(EXIT_FAILURE);
    }

    [[nodiscard]] std::tuple<bool, std::size_t>
    runtime_asset_manager::_find_loaded_asset_index(const asset_id id) noexcept
    {
        if (const auto found = std::find(
                std::cbegin(_loaded_assets.guids), std::cend(_loaded_assets.guids), id);
            found != std::cend(_loaded_assets.guids))
        {
            return { true, std::distance(std::cbegin(_loaded_assets.guids), found) };
        }
        return { false, std::numeric_limits<std::size_t>::max() };
    }

    [[nodiscard]] std::tuple<bool, std::size_t>
    runtime_asset_manager::_find_loaded_bundle_index(const asset_bundle_id b) noexcept
    {
        if (const auto found = std::find(
                std::cbegin(_loaded_bundles.guids), std::cend(_loaded_bundles.guids), b);
            found != std::cend(_loaded_bundles.guids))
        {
            return { true, std::distance(std::cbegin(_loaded_bundles.guids), found) };
        }
        return { false, std::numeric_limits<std::size_t>::max() };
    }

    void runtime_asset_manager::_load_bundle_metadata(std::ifstream& ifs);

    [[nodiscard]] std::vector<std::byte>
    runtime_asset_manager::_load_raw_asset() noexcept;

    void runtime_asset_manager::_execute_bundle_requests() noexcept
    {
        for (const auto& r : _bundle_load_requests)
        {
            const auto [found, i] = _find_loaded_bundle_index(r.bundle);
            if (found)
                ++_loaded_bundles.refcount[i];
            else
            {
                _load_bundle_metadata();
            }
        }

        for (const auto& r : _bundle_unload_requests)
        {
            const auto [found, i] = _find_loaded_bundle_index(r.bundle);
            if (found)
            {
                if (const auto refcount = --_loaded_bundles.refcount[i];
                    refcount == 0)
                    _unload_bundle_metadata();
            }
        }
    }

    runtime_asset_manager::runtime_asset_manager(const bundle_soa& bundles)
        : _available_bundles{ bundles.guids, bundles.names }
    {
        DRAKO_ASSERT(!std::empty(bundles.guids));
        DRAKO_ASSERT(!std::empty(bundles.names));
        DRAKO_ASSERT(!std::empty(bundles.paths));

        DRAKO_ASSERT(std::size(bundles.guids) == std::size(bundles.names));
        DRAKO_ASSERT(std::size(bundles.guids) == std::size(bundles.paths));

        _available_bundles.sources.reserve(std::size(bundles.paths));
        for (const auto& p : bundles.paths)
            _available_bundles.sources.emplace_back(p, std::ios::binary);
        _available_bundles.sources.shrink_to_fit();
    }

    [[nodiscard]] const asset_bundle&
    runtime_asset_manager::load_bundle(const asset_bundle_id id) noexcept
    {
        // search between loaded bundles
        if (const auto found = std::find(
                std::cbegin(_loaded_bundles.guids), std::cend(_loaded_bundles.guids), id);
            found != std::cend(_loaded_bundles.guids))
        { // bundle already loaded, just increment reference count
            const auto index = std::distance(std::cbegin(_loaded_bundles.guids), found);
            ++_loaded_bundles.refcount[index];
            return;
        }

        // retrieve bundle metadata
        if (const auto found = std::find(
                std::cbegin(_available_bundles.guids), std::cend(_available_bundles.guids), id);
            found != std::cend(_available_bundles.guids))
        {
            const auto index = std::distance(std::cbegin(_available_bundles.guids), found);
        }
        else
            _log_and_exit("Can't locate asset bundle with id " + std::to_string(id));
    }

    void runtime_asset_manager::load_bundle_async(const asset_bundle_id id) noexcept
    {
        DRAKO_ASSERT(valid(id));

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

    void runtime_asset_manager::unload_bundle(const asset_bundle_id id)
    {
        DRAKO_ASSERT(valid(id));

        _bundle_load_request request{};
        _bundle_unload_requests.push_back(request);
    }

    void runtime_asset_manager::update() noexcept
    {
        _execute_bundle_requests();
    }

} // namespace drako::engine