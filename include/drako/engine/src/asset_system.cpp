#include "drako/engine/asset_system.hpp"

#include "drako/devel/asset_bundle_manifest.hpp"
#include "drako/devel/logging.hpp"

#include <cassert>
#include <filesystem>
#include <vector>

namespace drako::engine
{
    [[nodiscard]] AssetSystemRuntime::_find_result
    AssetSystemRuntime::_available_bundle_index(const _bid id) noexcept
    {
        const auto& t = _available_bundles;
        if (const auto it = std::find(std::cbegin(t.ids), std::cend(t.ids), id);
            it != std::cend(t.ids))
            return { true, static_cast<std::size_t>(std::distance(std::cbegin(t.ids), it)) };
        else
            return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }

    [[nodiscard]] AssetSystemRuntime::_find_result
    AssetSystemRuntime::_loaded_asset_index(const _aid id) noexcept
    {
        const auto& t = _loaded_assets;
        if (const auto it = std::find(std::cbegin(t.ids), std::cend(t.ids), id);
            it != std::cend(t.ids))
            return { true, static_cast<std::size_t>(std::distance(std::cbegin(t.ids), it)) };
        else
            return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }

    [[nodiscard]] AssetSystemRuntime::_find_result
    AssetSystemRuntime::_loaded_bundle_index(const _bid b) noexcept
    {
        const auto& t = _loaded_bundles;
        if (const auto it = std::find(std::cbegin(t.ids), std::cend(t.ids), b);
            it != std::cend(t.ids))
            return { true, static_cast<std::size_t>(std::distance(std::cbegin(t.ids), it)) };
        else
            return { false, std::numeric_limits<decltype(_find_result::index)>::max() };
    }


    void AssetSystemRuntime::_handle_bundle_requests() noexcept
    {
        /*
        // gather completed loads
        std::vector<_request_id> completed;
        for (auto i = 0; i < std::size(_io_outputs); ++i)
            while (const auto id = _io_outputs[i].pop())
                completed.push_back(id.value());

        std::vector<std::span<std::byte>> payloads;
        for (auto request : completed)
        {
            // TODO: separate bundles and assets
        }


        // increase reference count of already loaded bundles
        std::vector<_bid> not_loaded;
        for (const auto& id : _bundle_load_list)
        {
            if (const auto [found, i] = _loaded_bundle_index(id); found)
                ++_loaded_bundles.refcount[i];
            else
                not_loaded.push_back(id);
        }
        _bundle_load_list.clear();

        // increase reference count of already pending bundles
        std::sort(std::begin(not_loaded), std::end(not_loaded));
        std::sort(std::begin(_load_in_flight), std::end(_load_in_flight));

        std::vector<_bid> not_in_flight;
        std::set_difference(std::begin(not_loaded), std::end(not_loaded),
            std::begin(_load_in_flight), std::end(_load_in_flight), std::begin(not_in_flight));

        for (auto bundle : not_in_flight) // TODO: for the ones in flight, increment refcount
        {
            if (const auto [found, i] = _available_bundle_index(bundle); found)
            {
                auto&      file   = _available_bundles.sources[i];
                const auto size   = _available_bundles.sizes[i];
                auto       buffer = std::make_unique<std::byte[]>(size);

                //assert(_ioservice.submit(file, std::span{ buffer.get(), size }, {}));
            }
        }

        for (auto id : _bundle_dump_list)
        {
            if (const auto [found, i] = _loaded_bundle_index(id); found)
            {
                if (const auto refc = --_loaded_bundles.refcount[i]; refc == 0)
                { // unload bundle
                    _loaded_bundles.ids[i] = _bid{ 0 };
                    // _loaded_bundles.bundles[i]

                    // TODO: end impl
                }
            }
        }
        _bundle_dump_list.clear();
        */
    }

    void AssetSystemRuntime::_handle_asset_requests() noexcept
    {

    }

    AssetSystemRuntime::AssetSystemRuntime(const BundlesArgs& bundles, const ConfigArgs& config)
        : _config{ config }
        , _io_service{ { .workers = 4, .submit_queue_size = 100, .output_queue_size = 100 } }
        , _asset_requests_pool{ 100 }
        , _bundle_requests_pool{ 100 }
    {
        namespace _fs = std::filesystem;

        assert(!std::empty(bundles.ids));
        assert(!std::empty(bundles.names));
        assert(std::size(bundles.ids) == std::size(bundles.names));

        assert(_fs::is_directory(config.bundle_manifest_directory));
        assert(_fs::is_directory(config.bundle_data_directory));

        /*
        { // load serialized bundle list
            const _fs::path bundle_list = "main_bundle_list.dk";
            const auto      size        = static_cast<std::size_t>(_fs::file_size(bundle_list));
            auto            buffer      = std::make_unique<std::byte[]>(size);
            {
                io::UniqueInputFile file{ bundle_list };
                file.read(buffer.get(), size);
            }
            const AssetBundleListView view{ { buffer.get(), size } };
            _available_bundles.ids.resize(std::size(view));
            std::memcpy(std::data(_available_bundles.ids),
                std::data(view.ids()), view.ids().size_bytes());
        }*/

        _available_bundles.ids   = bundles.ids;
        _available_bundles.names = bundles.names;
        //_available_bundles.ids.reserve(std::size(bundles.ids));
        //_available_bundles.sources.reserve(std::size(bundles.ids));
        for (const auto& id : bundles.ids)
        {
            const auto path = manifest_filename(id);
            const auto size = static_cast<std::size_t>(_fs::file_size(path));
            _available_bundles.sources.emplace_back(path);
            _available_bundles.sizes.push_back(size);
        }
        //_available_bundles.sources.shrink_to_fit();
        //_available_bundles.sizes.shrink_to_fit();
    }

    void AssetSystemRuntime::update() noexcept
    {
        _handle_bundle_requests();
        _handle_asset_requests();
    }

    void AssetSystemRuntime::debug_print_registered_bundles()
    {
        std::cout << "[available_bundles]\n[id]\t\t[name]\t\t[size(bytes)]\n";
        const auto& t = _available_bundles;
        for (auto i = 0; i < std::size(t.ids); ++i)
            std::cout << t.ids[i] << '\t\t'
                      << t.names[i] << '\t\t'
                      << t.sizes[i] << '\n';
    }

    void AssetSystemRuntime::debug_print_loaded_bundles()
    {
        std::cout << "[loaded_bundles]\n[id]\t\t[references]\n";
        const auto& t = _loaded_bundles;
        for (auto i = 0; i < std::size(t.ids); ++i)
            std::cout << t.ids[i] << '\t\t'
                      << t.refcount[i] << '\n';
    }

    void AssetSystemRuntime::debug_print_assets()
    {
        std::cout << "Loaded assets (ID | refcount):\n";
        const auto& t = _loaded_assets;
        for (auto i = 0; i < std::size(t.ids); ++i)
            std::cout << t.ids[i] << ' ' << t.refcount[i] << '\n';
    }

} // namespace drako::engine