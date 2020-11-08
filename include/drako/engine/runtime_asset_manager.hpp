#pragma once
#ifndef DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP
#define DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP

#include "drako/devel/asset_types.hpp"
#include "drako/devel/asset_utils.hpp"
#include "drako/graphics/mesh_types.hpp"

#include <filesystem>
#include <iosfwd>
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
    class runtime_asset_manager
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

        explicit runtime_asset_manager(const bundle_manifest_soa&, const config&);

        runtime_asset_manager(const runtime_asset_manager&) = delete;
        runtime_asset_manager& operator=(const runtime_asset_manager&) = delete;


        /// @brief Loads the bundle into memory.
        [[nodiscard]] const asset_bundle& load_bundle(const asset_bundle_id id) noexcept;
        [[nodiscard]] const asset_bundle& load_bundle(const std::string_view name) noexcept;
        void                              load_bundle(const std::vector<asset_bundle_id>& ids) noexcept;
        void                              load_bundle(const std::vector<std::string_view>& names) noexcept;

        void load_bundle_async(const asset_bundle_id id) noexcept;

        /// @brief Unloads the bundle.
        ///
        /// This request will be satisfied only if the bundle is no longer needed.
        ///
        void unload_bundle(const asset_bundle_id id) noexcept;
        void unload_bundle(const std::vector<asset_bundle_id>& ids) noexcept;


        void load_asset(const uuid& id) noexcept;
        void load_asset(const std::vector<uuid>& ids) noexcept;

        /// @brief Executes pending asynchronous requests.
        void update() noexcept;

        /// @brief Prints a table of currently loaded bundles.
        void dbg_print_loaded_bundles(std::ostream os);

    private:
        struct _bundle_load_request
        {
            asset_bundle_id id;
        };

        struct _bundle_unload_request
        {
            asset_bundle_id id;
        };

        const config _config;

        // TODO: vvv those needs to be threadsafe vvv

        std::vector<_bundle_load_request>   _bundle_load_requests;
        std::vector<_bundle_unload_request> _bundle_unload_requests;

        // TODO: ^^^ those needs to be threadsafe ^^^

        struct _available_bundles_table
        {
            std::vector<asset_bundle_id> ids;
            std::vector<std::ifstream>   sources;
            std::vector<std::size_t>     sizes;
            std::vector<std::string>     names;
        };
        _available_bundles_table _available_bundles;

        struct _loaded_bundles_table
        {
            std::vector<asset_bundle_id> ids;
            std::vector<asset_bundle>    bundles;
            std::vector<std::uint16_t>   refcount;
        };
        _loaded_bundles_table _loaded_bundles;

        struct _loaded_asset_table
        {
            std::vector<uuid>                         ids;
            std::vector<std::unique_ptr<std::byte[]>> data;
            std::vector<std::uint16_t>                refcount;
        };
        _loaded_asset_table _loaded_assets;



        void _increment_ref_count(const asset_id asset) noexcept;
        void _decrement_ref_count(const asset_id asset) noexcept;

        struct _find_result
        {
            bool   found;
            size_t index;
        };
        [[nodiscard]] _find_result _available_bundle_index(const asset_bundle_id) noexcept;
        [[nodiscard]] _find_result _loaded_asset_index(const asset_id) noexcept;
        [[nodiscard]] _find_result _loaded_bundle_index(const asset_bundle_id) noexcept;

        void _load_bundle_metadata(const asset_bundle_id);

        void _unload_bundle_metadata();

        [[nodiscard]] std::vector<std::byte>
        _load_raw_asset(const asset_load_info& info) noexcept;

        void _dispatch_bundle_requests() noexcept;
    };

} // namespace drako::engine

#endif // !DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP