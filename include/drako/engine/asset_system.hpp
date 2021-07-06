#pragma once
#ifndef DRAKO_ASSET_SYSTEM_HPP
#define DRAKO_ASSET_SYSTEM_HPP

#include "drako/concurrency/async_reader_pool.hpp"
#include "drako/concurrency/lockfree_ringbuffer.hpp"
#include "drako/core/memory/unsync_pool_allocator.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/asset_utils.hpp"
#include "drako/graphics/mesh_types.hpp"

#include <rio/input_file_handle.hpp>

#include <cassert>
#include <filesystem>
#include <functional>
#include <vector>

namespace drako::engine
{
    struct AssetLoadRequest
    {
        std::span<const AssetID> assets;
        std::function<void()>    callback;
    };


    /// @brief Runtime manager of loaded assets.
    class AssetSystemRuntime
    {
    public:
        struct BundlesArgs
        {
            std::vector<AssetBundleID> ids;
            std::vector<std::string>   names;
        };

        struct ConfigArgs
        {
            /// @brief Directory where asset bundles manifests are located.
            std::filesystem::path bundle_manifest_directory;

            /// @brief Directory where asset bundles data archives are located.
            std::filesystem::path bundle_data_directory;
        };

        //using bundle_loaded_callback = void(*)();

        explicit AssetSystemRuntime(const BundlesArgs&, const ConfigArgs&);

        AssetSystemRuntime(const AssetSystemRuntime&) = delete;
        AssetSystemRuntime& operator=(const AssetSystemRuntime&) = delete;


        /// @brief Submit a request.
        /// @param id Bundle to load.
        ///
        //void load_bundle(const AssetBundleID id) noexcept;

        /// @brief Unloads the bundle.
        ///
        /// Notify that a bundle is no longer required.
        ///
        //void unload_bundle(const AssetBundleID id) noexcept;

        void load_asset(const AssetID) noexcept;
        void load_asset(const AssetLoadRequest&) noexcept;

        void unload_asset(const AssetID) noexcept;
        //void unload_asset(std::span<const AssetID>) noexcept;

        /// @brief Executes pending asynchronous requests.
        void update() noexcept;

        /// @brief Prints a table of currently registered bundles.
        void debug_print_registered_bundles();

        /// @brief Prints a table of currently loaded bundles.
        void debug_print_loaded_bundles();

        /// @brief Print a table of loaded assets.
        void debug_print_assets();

    private:
        using _aid        = AssetID;
        using _bid        = AssetBundleID;
        using _request_id = std::uint32_t;

        const ConfigArgs _config;

        AsyncReaderPool _io_service;

        // TODO: vvv those needs to be threadsafe vvv
        std::vector<AssetBundleID> _bundle_load_list; // load requests
        std::vector<AssetBundleID> _bundle_dump_list; // unload requests
        // TODO: ^^^ those needs to be threadsafe ^^^

        // TODO: vvv those needs to be threadsafe vvv
        std::vector<AssetID> _asset_load_list; // load requests
        std::vector<AssetID> _asset_dump_list; // unload requests
        // TODO: ^^^ those needs to be threadsafe ^^^

        struct _pending_bundle_request
        {
            _request_id          request;
            AssetBundleID        id;       // bundle that is being loaded
            std::span<std::byte> data;     // destination for disk read
            std::uint32_t        refcount; // active references to the bundle
        };

        std::vector<AssetBundleID>            _load_in_flight; // loading operation in flight
        std::vector<_pending_bundle_request*> _load_requests;  // associated request

        struct _pending_asset_request
        {
            _request_id          id;
            AssetID              asset;
            std::span<std::byte> data;
            std::uint32_t        refcount;
        };

        Pool<_pending_asset_request>  _asset_requests_pool;
        Pool<_pending_bundle_request> _bundle_requests_pool; // local allocator for requests


        struct _available_bundles_table
        {
            std::vector<AssetBundleID>        ids;
            std::vector<rio::UniqueInputFile> sources;
            std::vector<std::size_t>          sizes;
            std::vector<std::string>          names;
        } _available_bundles;

        struct _loaded_bundles_table
        {
            std::vector<AssetBundleID>       ids;
            std::vector<AssetBundleManifest> manifests;
            std::vector<std::uint16_t>       refcount;
        } _loaded_bundles;

        struct _loaded_assets_table
        {
            std::vector<AssetID>                      ids;
            std::vector<std::unique_ptr<std::byte[]>> data;
            std::vector<std::uint16_t>                refcount;
        } _loaded_assets;

        struct _find_result
        {
            bool        found;
            std::size_t index;
        };
        [[nodiscard]] _find_result _available_bundle_index(const _bid) noexcept;
        [[nodiscard]] _find_result _loaded_asset_index(const _aid) noexcept;
        [[nodiscard]] _find_result _loaded_bundle_index(const _bid) noexcept;

        void _handle_bundle_requests() noexcept;
        void _handle_asset_requests() noexcept;
    };

    /*
    inline void AssetSystemRuntime::load_bundle(const AssetBundleID id) noexcept
    {
        assert(id);
        _bundle_load_list.push_back(id);
    }

    inline void AssetSystemRuntime::unload_bundle(const AssetBundleID id) noexcept
    {
        assert(id);
        _bundle_dump_list.push_back(id);
    }
    */

    inline void AssetSystemRuntime::load_asset(const AssetID a) noexcept
    {
        assert(a);
        _asset_load_list.push_back(a);
    }

    inline void AssetSystemRuntime::unload_asset(const AssetID a) noexcept
    {
        assert(a);
        _asset_dump_list.push_back(a);
    }

} // namespace drako::engine

#endif // !DRAKO_ASSET_SYSTEM_HPP