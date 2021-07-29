#include "drako/engine/asset_system.hpp"

#include "drako/devel/asset_bundle_types.hpp"
#include "drako/devel/project_utils.hpp"

#include <rio/input_file_handle.hpp>

#include <cassert>
#include <filesystem>
#include <vector>

namespace drako::engine
{
    // convert a list of IDs to a list of indices
    [[nodiscard]] std::vector<std::size_t> _id_to_index(
        const std::vector<AssetID>& table, const std::vector<AssetID>& assets)
    {
        std::vector<std::size_t> indices;
        indices.reserve(std::size(assets));

        for (const auto& a : assets)
        {
            const auto it = std::find(std::cbegin(table), std::cend(table), a);
            assert(it != std::cend(table));
            indices.push_back(std::distance(std::cbegin(table), it));
        }
        return indices;
    }


    bool AssetSystemRuntime::_loaded(const AssetID id) noexcept
    {
        const auto& t = _loaded_assets.ids;
        return std::find(std::cbegin(t), std::cend(t), id) != std::cend(t);
    }

    bool AssetSystemRuntime::_pending(const AssetID id) noexcept
    {
        const auto& t = _pending_assets.ids;
        return std::find(std::cbegin(t), std::cend(t), id) != std::cend(t);
    }

    void AssetSystemRuntime::_inc_ref_count(const AssetID id) noexcept
    {
        auto&      t     = _loaded_assets;
        const auto it    = std::find(std::cbegin(t.ids), std::cend(t.ids), id);
        const auto index = std::distance(std::cbegin(t.ids), it);
        ++t.refcount[index];
    }


    void AssetSystemRuntime::_handle_bundle_requests()
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

    void AssetSystemRuntime::_handle_asset_requests()
    {
        for (const auto& request : _asset_load_requests)
        {
            std::vector<AssetID> assets_to_load;
            for (const auto& asset : request.assets)
            {
                if (_loaded(asset))
                    _inc_ref_count(asset);
                else
                {
                    if (_pending(asset))
                        _inc_ref_count(asset);
                    else
                        assets_to_load.push_back(asset);
                }
            }

            if (!std::empty(assets_to_load))
            {
                // set up a batch handle
                //auto batch      = _batch_handles_pool.allocate(1);
                //batch->callback = request.callback;
                //batch->counter  = std::size(assets_to_load);
                //batch->handles.reserve(std::size(assets_to_load));

                const auto indices = _id_to_index(_loaded_assets.ids, assets_to_load);

                for (const auto& i : indices)
                    assert(!_assets.data[i]); // asset is not loaded

//#pragma omp parallel for
                for (const auto& i : indices)
                {
                    const auto& path = _config.asset_data_directory /
                                       editor::guid_to_datafile(_assets.ids[i]);
                    rio::UniqueInputFile file{ path };

                    const auto& meta = _assets.meta[i];
                    _assets.data[i]  = std::make_unique_for_overwrite<std::byte[]>(meta.packed_size_bytes());

                    rio::read_exact(file, { _assets.data[i].get(), meta.packed_size_bytes() });
                    _assets.refcount[i] = 1;
                }
            }
        }
    }

    AssetSystemRuntime::AssetSystemRuntime(const BundlesArgs& bundles, const ConfigArgs& config)
        : _config{ config }
    //, _io_service{ { .workers = 4, .submit_queue_size = 100, .output_queue_size = 100 } }
    //, _asset_requests_pool{ 100 }
    //, _bundle_requests_pool{ 100 }
    {
        namespace _fs = std::filesystem;

        assert(!std::empty(bundles.ids));
        assert(!std::empty(bundles.names));
        assert(std::size(bundles.ids) == std::size(bundles.names));

        assert(_fs::is_directory(config.asset_data_directory));
        assert(_fs::is_directory(config.bundle_data_directory));
        assert(_fs::is_directory(config.bundle_meta_directory));

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
            const auto path = bundle_meta_filename(id);
            const auto size = static_cast<std::size_t>(_fs::file_size(path));
            _available_bundles.sources.emplace_back(path);
            _available_bundles.sizes.push_back(size);
        }
        //_available_bundles.sources.shrink_to_fit();
        //_available_bundles.sizes.shrink_to_fit();
    }

    void AssetSystemRuntime::update()
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

    [[nodiscard]] bool AssetSystemRuntime::debug_check_asset_loaded(std::span<const AssetID> s) noexcept
    {
        const auto& ids = _loaded_assets.ids;
        for (const auto& asset : s)
            if (std::find(std::cbegin(ids), std::cend(ids), asset) == std::cend(ids))
                return false;
        return true;
    }

    [[nodiscard]] bool AssetSystemRuntime::debug_check_bundle_loaded(std::span<const AssetBundleID> s) noexcept
    {
        const auto& ids = _loaded_bundles.ids;
        for (const auto& bundle : s)
            if (std::find(std::cbegin(ids), std::cend(ids), bundle) == std::cend(ids))
                return false;
        return true;
    }

} // namespace drako::engine