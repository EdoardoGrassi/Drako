#include "drako/engine/asset_manager.hpp"

#include "drako/devel/asset_bundle_manifest.hpp"
#include "drako/devel/logging.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>

namespace drako::engine
{
    using _this = asset_manager;

    [[nodiscard]] _this::_find_result _this::_available_bundle_index(const _bid id) noexcept
    {
        const auto& t = _available_bundles;
        if (const auto it = std::find(std::cbegin(t.ids), std::cend(t.ids), id);
            it != std::cend(t.ids))
            return { true, std::distance(std::cbegin(t.ids), it) };
        else
            return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }

    [[nodiscard]] _this::_find_result _this::_loaded_asset_index(const _aid id) noexcept
    {
        const auto& t = _loaded_assets;
        if (const auto it = std::find(std::cbegin(t.ids), std::cend(t.ids), id);
            it != std::cend(t.ids))
            return { true, std::distance(std::cbegin(t.ids), it) };
        else
            return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }

    [[nodiscard]] _this::_find_result _this::_loaded_bundle_index(const asset_bundle_id b) noexcept
    {
        const auto& t = _loaded_bundles;
        if (const auto it = std::find(std::cbegin(t.ids), std::cend(t.ids), b);
            it != std::cend(t.ids))
            return { true, std::distance(std::cbegin(t.ids), it) };
        else
            return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }

    void _this::_load_bundle_metadata(const _bid id, const size_t index)
    {
        auto&      fs   = _available_bundles.sources[index];
        const auto size = _available_bundles.sizes[index];

        auto memory = std::make_unique<std::byte[]>(size);
        fs.read(reinterpret_cast<char*>(memory.get()), size);

        _loaded_bundles.ids.push_back(id);
        _loaded_bundles.refcount.push_back(1);
        _loaded_bundles.bundles.push_back();
    }

    //[[nodiscard]] std::vector<std::byte> _this::_load_raw_asset() noexcept;

    void _this::_dispatch_bundle_requests() noexcept
    {
        std::vector<_bid> not_loaded;
        for (const auto& id : _bundle_load_list)
        {
            if (const auto [found, i] = _loaded_bundle_index(id); found)
                ++_loaded_bundles.refcount[i];
            else
                not_loaded.push_back(id);
        }

        std::vector<_bid> not_in_flight;

        std::sort(std::begin(not_loaded), std::end(not_loaded));
        std::sort(std::begin(_load_in_flight), std::end(_load_in_flight));
        std::set_difference(std::begin(not_loaded), std::end(not_loaded),
            std::begin(_load_in_flight), std::end(_load_in_flight), std::begin(not_in_flight));

        for (auto b : not_in_flight) // TODO: for the ones in flight, increment refcount
            void();



        for (const auto& id : _bundle_dump_list)
        {
            if (const auto [found, i] = _loaded_bundle_index(id); found)
            {
                if (const auto refc = --_loaded_bundles.refcount[i]; refc == 0)
                    _unload_bundle_metadata();
            }
        }

        for (const auto b : _bundle_load_complete)
        {
            manifest_view m{};
        }
    }

    _this::asset_manager(const bundle_manifest_soa& bundles, const config& config)
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
        {
            std::ifstream f{ p, std::ios::binary };
            f.exceptions(std::ios_base::failbit | std::ios_base::badbit);
            _available_bundles.sources.push_back(f);
        }
        _available_bundles.sources.shrink_to_fit();

        _available_bundles.sizes.reserve(std::size(bundles.paths));
        for (const auto& p : bundles.paths)
        {
            const auto size = static_cast<std::size_t>(_fs::file_size(p));
            _available_bundles.sizes.push_back(size);
        }
        _available_bundles.sizes.shrink_to_fit();
    }

    void _this::load_bundle_async(const asset_bundle_id id) noexcept
    {
        assert(id);
        _bundle_load_list.push_back(id);
    }

    void _this::unload_bundle(const asset_bundle_id id)
    {
        assert(id);
        _bundle_dump_list.push_back(id);
    }

    void _this::load_asset(const uuid& a) noexcept
    {
        assert(a.has_value());
        _asset_load_list.push_back(a);
    }

    void _this::unload_asset(const uuid& a) noexcept
    {
        assert(a.has_value());
        _asset_dump_list.push_back(a);
    }

    void _this::update() noexcept
    {
        _dispatch_bundle_requests();
    }

    void _this::debug_print_loaded_bundles()
    {
        std::cout << "[available_bundles]\n[id]\t\t[name]\t\t[size(bytes)]\n";
        const auto& t = _available_bundles;
        for (auto i = 0; i < std::size(t.ids); ++i)
            std::cout << t.ids[i] << '\t\t'
                      << t.names[i] << '\t\t'
                      << t.sizes[i] << '\n';
    }

    void _this::debug_print_loaded_bundles()
    {
        std::cout << "[loaded_bundles]\n[id]\t\t[references]\n";
        const auto& t = _loaded_bundles;
        for (auto i = 0; i < std::size(t.ids); ++i)
            std::cout << t.ids[i] << '\t\t'
                      << t.refcount[i] << '\n';
    }

} // namespace drako::engine