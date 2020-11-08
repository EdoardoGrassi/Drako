#include "drako/engine/runtime_asset_manager.hpp"

#include "drako/devel/logging.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>

namespace drako::engine
{
    using _this = runtime_asset_manager;
    using _aid  = asset_id;
    using _bid  = asset_bundle_id;

    [[nodiscard]] _this::_find_result _this::_available_bundle_index(const _bid id) noexcept
    {
        if (const auto it = std::find(
                std::cbegin(_available_bundles.ids), std::cend(_available_bundles.ids), id);
            it != std::cend(_available_bundles.ids))
        {
            return { true, std::distance(std::cbegin(_available_bundles.ids), it) };
        }
        else
            return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }

    [[nodiscard]] _this::_find_result _this::_loaded_asset_index(const asset_id id) noexcept
    {
        if (const auto it = std::find(
                std::cbegin(_loaded_assets.ids), std::cend(_loaded_assets.ids), id);
            it != std::cend(_loaded_assets.ids))
        {
            return { true, std::distance(std::cbegin(_loaded_assets.ids), it) };
        }
        return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }

    [[nodiscard]] _this::_find_result _this::_loaded_bundle_index(const asset_bundle_id b) noexcept
    {
        if (const auto it = std::find(
                std::cbegin(_loaded_bundles.ids), std::cend(_loaded_bundles.ids), b);
            it != std::cend(_loaded_bundles.ids))
        {
            return { true, std::distance(std::cbegin(_loaded_bundles.ids), it) };
        }
        return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }

    void _this::_load_bundle_metadata(const _bid id)
    {
        if (const auto [found, i] = _available_bundle_index(id); found)
        {
            auto&      fs   = _available_bundles.sources[i];
            const auto size = _available_bundles.sizes[i];

            auto memory = std::make_unique<std::byte[]>(size);
            fs.exceptions(std::ios_base::failbit | std::ios_base::badbit);
            fs.read(reinterpret_cast<char*>(memory.get()), size);

            _loaded_bundles.ids.emplace_back(id);
            _loaded_bundles.refcount.emplace_back(0);
        }
        else
            throw std::runtime_error{ "Can't locate bundle with id <" + std::to_string(id) + ">." };
    }

    //[[nodiscard]] std::vector<std::byte> _this::_load_raw_asset() noexcept;

    void _this::_dispatch_bundle_requests() noexcept
    {
        for (const auto& r : _bundle_load_requests)
        {
            if (const auto [found, i] = _loaded_bundle_index(r.id); found)
                ++_loaded_bundles.refcount[i];
            else
                _load_bundle_metadata(i);
        }

        for (const auto& r : _bundle_unload_requests)
        {
            if (const auto [found, i] = _loaded_bundle_index(r.id); found)
            {
                if (const auto refc = --_loaded_bundles.refcount[i]; refc == 0)
                    _unload_bundle_metadata();
            }
        }
    }

    _this::runtime_asset_manager(const bundle_manifest_soa& bundles, const config& config)
        : _config{ config }
    {
        namespace _fs = std::filesystem;

        assert(!std::empty(bundles.ids));
        assert(!std::empty(bundles.names));
        assert(!std::empty(bundles.paths));

        assert(std::size(bundles.ids) == std::size(bundles.names));
        assert(std::size(bundles.ids) == std::size(bundles.paths));

        assert(_fs::is_directory(config.bundle_manifest_directory));
        assert(_fs::is_directory(config.bundle_data_directory));

        _available_bundles.ids   = bundles.ids;
        _available_bundles.names = bundles.names;

        _available_bundles.sources.reserve(std::size(bundles.paths));
        for (const auto& p : bundles.paths)
            _available_bundles.sources.emplace_back(p, std::ios::binary);
        _available_bundles.sources.shrink_to_fit();

        _available_bundles.sizes.reserve(std::size(bundles.paths));
        for (const auto& p : bundles.paths)
            _available_bundles.sizes.emplace_back(static_cast<size_t>(_fs::file_size(p)));
        _available_bundles.sizes.shrink_to_fit();
    }

    [[nodiscard]] const asset_bundle& _this::load_bundle(const asset_bundle_id id) noexcept
    {
        // search between loaded bundles
        if (const auto found = std::find(
                std::cbegin(_loaded_bundles.ids), std::cend(_loaded_bundles.ids), id);
            found != std::cend(_loaded_bundles.ids))
        { // bundle already loaded, just increment reference count
            const auto index = std::distance(std::cbegin(_loaded_bundles.ids), found);
            ++_loaded_bundles.refcount[index];
            return;
        }

        // retrieve bundle metadata
        if (const auto found = std::find(
                std::cbegin(_available_bundles.ids), std::cend(_available_bundles.ids), id);
            found != std::cend(_available_bundles.ids))
        {
            const auto index = std::distance(std::cbegin(_available_bundles.ids), found);
        }
        else
            log_and_exit("Can't locate asset bundle with id " + std::to_string(id));
    }

    void _this::load_bundle_async(const asset_bundle_id id) noexcept
    {
        assert(valid(id));

        _bundle_load_request request{ .id = id };
        _bundle_load_requests.push_back(request);
    }

    /*
    [[nodiscard]] const asset_bundle& _this::load_asset_bundle(const std::string_view name) noexcept
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

    void _this::unload_bundle(const asset_bundle_id id)
    {
        assert(valid(id));

        _bundle_unload_request request{ .id = id };
        _bundle_unload_requests.push_back(request);
    }

    void _this::update() noexcept
    {
        _dispatch_bundle_requests();
    }

} // namespace drako::engine