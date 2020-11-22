#pragma once
#ifndef DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP
#define DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP

#include "drako/devel/asset_types.hpp"
#include "drako/devel/asset_utils.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/io/reader_service_pool.hpp"

#include <atomic>
#include <filesystem>
#include <iosfwd>
#include <thread>
#include <vector>

namespace drako::engine
{
    struct asset_context
    {
        struct _mesh_table
        {
            std::vector<uint32_t> ids;
            std::vector<mesh>     assets;
        };
        _mesh_table mesh_table;
    };


    //template <typename ...Assets>
    class asset_manager
    {
    public:
        struct bundle_manifest_soa
        {
            std::vector<asset_bundle_id>       ids;
            std::vector<std::string>           names;
            std::vector<std::filesystem::path> paths;
        };

        struct config
        {
            /// @brief Directory where asset bundles manifests are located.
            std::filesystem::path bundle_manifest_directory;

            /// @brief Directory where asset bundles data archives are located.
            std::filesystem::path bundle_data_directory;
        };

        //using bundle_loaded_callback = void(*)();

        explicit asset_manager(const bundle_manifest_soa&, const config&);

        asset_manager(const asset_manager&) = delete;
        asset_manager& operator=(const asset_manager&) = delete;

        void load_bundle_async(const asset_bundle_id id) noexcept;

        /// @brief Unloads the bundle.
        ///
        /// This request will be satisfied only if the bundle is no longer needed.
        ///
        void unload_bundle(const asset_bundle_id id) noexcept;
        //void unload_bundle(const std::vector<asset_bundle_id>& ids) noexcept;

        void load_asset(const uuid&) noexcept;
        //void load_asset(std::span<const uuid>) noexcept;

        void unload_asset(const uuid&) noexcept;
        //void unload_asset(std::span<const uuid>) noexcept;

        /// @brief Executes pending asynchronous requests.
        void update() noexcept;

        /// @brief Prints a table of currently registered bundles.
        void debug_print_registered_bundles();

        /// @brief Prints a table of currently loaded bundles.
        void debug_print_loaded_bundles();

    private:
        using _aid = uuid;
        using _bid = asset_bundle_id;

        const config _config;

        io::ReaderPool _pool;

        // TODO: vvv those needs to be threadsafe vvv

        std::vector<_bid> _bundle_load_list; // load requests
        std::vector<_bid> _bundle_dump_list; // unload requests
        std::vector<_bid> _load_in_flight;   // loading operation in flight
        std::vector<_bid> _bundle_load_complete;

        struct _pending_bundle
        {
            _bid                 id;
            std::span<std::byte> data;
        };
        std::vector<_pending_bundle> _pending_create;

        std::vector<_aid> _asset_load_list;
        std::vector<_aid> _asset_dump_list;
        // TODO: ^^^ those needs to be threadsafe ^^^

        struct _available_bundles_table
        {
            std::vector<_bid>          ids;
            std::vector<std::ifstream> sources;
            std::vector<std::size_t>   sizes;
            std::vector<std::string>   names;
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
            std::vector<uuid>                         ids;
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

        void _load_bundle_metadata(const _bid, const std::size_t);

        void _unload_bundle_metadata();

        [[nodiscard]] std::vector<std::byte>
        _load_raw_asset(const asset_load_info& info) noexcept;

        void _dispatch_bundle_requests() noexcept;
    };

} // namespace drako::engine

#endif // !DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP