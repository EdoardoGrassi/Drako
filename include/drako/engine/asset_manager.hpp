#pragma once
#ifndef DRAKO_ASSET_MANAGER_HPP
#define DRAKO_ASSET_MANAGER_HPP

#include "drako/concurrency/lockfree_ringbuffer.hpp"
#include "drako/core/memory/unsync_pool_allocator.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/asset_utils.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/io/async_reader_pool.hpp"
#include "drako/io/input_file_handle.hpp"

#include <cassert>
#include <filesystem>
#include <vector>

namespace drako::engine
{
    //template <typename ...Assets>
    class AssetManager
    {
    public:
        struct bundle_manifest_soa
        {
            std::vector<AssetBundleID>         ids;
            std::vector<std::string>           names;
            std::vector<std::filesystem::path> paths;
        };

        struct Config
        {
            /// @brief Directory where asset bundles manifests are located.
            std::filesystem::path bundle_manifest_directory;

            /// @brief Directory where asset bundles data archives are located.
            std::filesystem::path bundle_data_directory;
        };

        //using bundle_loaded_callback = void(*)();

        explicit AssetManager(const bundle_manifest_soa&, const Config&);

        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;


        /// @brief Submit a request.
        /// @param id Bundle to load.
        ///
        void load_bundle(const AssetBundleID id) noexcept;

        /// @brief Unloads the bundle.
        ///
        /// Notify that a bundle is no longer required.
        ///
        void unload_bundle(const AssetBundleID id) noexcept;
        //void unload_bundle(const std::vector<asset_bundle_id>& ids) noexcept;

        void load_asset(const AssetID) noexcept;
        //void load_asset(std::span<const uuid>) noexcept;

        void unload_asset(const AssetID) noexcept;
        //void unload_asset(std::span<const uuid>) noexcept;

        /// @brief Executes pending asynchronous requests.
        void update() noexcept;

        /// @brief Prints a table of currently registered bundles.
        void debug_print_registered_bundles();

        /// @brief Prints a table of currently loaded bundles.
        void debug_print_loaded_bundles();

    private:
        using _aid        = AssetID;
        using _bid        = AssetBundleID;
        using _request_id = std::uint32_t;

        const Config _config;

        io::AsyncReaderPool _io_service;

        // TODO: vvv those needs to be threadsafe vvv
        std::vector<_bid> _bundle_load_list; // load requests
        std::vector<_bid> _bundle_dump_list; // unload requests
        // TODO: ^^^ those needs to be threadsafe ^^^

        // TODO: vvv those needs to be threadsafe vvv
        std::vector<_aid> _asset_load_list; // load requests
        std::vector<_aid> _asset_dump_list; // unload requests
        // TODO: ^^^ those needs to be threadsafe ^^^

        struct _pending_request
        {
            _request_id          request;
            _bid                 id;       // bundle that is being loaded
            std::span<std::byte> data;     // destination for disk read
            std::uint16_t        refcount; // active references to the bundle
        };

        std::vector<_bid>              _load_in_flight; // loading operation in flight
        std::vector<_pending_request*> _load_requests;  // associated request

        Pool<_pending_request> _requests; // local allocator for requests


        struct _available_bundles_table
        {
            std::vector<_bid>                  ids;
            std::vector<io::UniqueInputFile> sources;
            std::vector<std::size_t>           sizes;
            std::vector<std::string>           names;
        } _available_bundles;

        struct _loaded_bundles_table
        {
            std::vector<_bid>          ids;
            std::vector<asset_bundle>  bundles;
            std::vector<std::uint16_t> refcount;
        } _loaded_bundles;

        struct _metadata_table
        {
        } _metadata;

        struct _loaded_assets_table
        {
            std::vector<_aid>                         ids;
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

    inline void AssetManager::load_bundle(const AssetBundleID id) noexcept
    {
        assert(id);
        _bundle_load_list.push_back(id);
    }

    inline void AssetManager::unload_bundle(const AssetBundleID id) noexcept
    {
        assert(id);
        _bundle_dump_list.push_back(id);
    }

    inline void AssetManager::load_asset(const AssetID a) noexcept
    {
        assert(a);
        _asset_load_list.push_back(a);
    }

    inline void AssetManager::unload_asset(const AssetID a) noexcept
    {
        assert(a);
        _asset_dump_list.push_back(a);
    }

} // namespace drako::engine

#endif // !DRAKO_ASSET_MANAGER_HPP