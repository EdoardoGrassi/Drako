#pragma once
#ifndef DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP
#define DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP

#include "drako/devel/assertion.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/asset_utils.hpp"

#include "drako/graphics/mesh_types.hpp"

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

    struct bundle_soa
    {
        std::vector<asset_bundle_id>       guids;
        std::vector<std::string>           names;
        std::vector<std::filesystem::path> paths;
    };

    //template <typename ...Assets>
    class runtime_asset_manager
    {
    public:
        // explicit runtime_asset_manager(const std::vector<asset_bundle>& bundles);

        explicit runtime_asset_manager(const bundle_soa& bundles);

        runtime_asset_manager(const runtime_asset_manager&) = delete;
        runtime_asset_manager& operator=(const runtime_asset_manager&) = delete;

        runtime_asset_manager(runtime_asset_manager&&) = delete;
        runtime_asset_manager& operator=(runtime_asset_manager&&) = delete;

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

        template <typename T>
        [[nodiscard]] T load_asset(const asset_bundle_id bundle, const uuid& id) noexcept;

        void load_asset(const uuid& id) noexcept;
        void load_asset(const std::vector<uuid>& ids) noexcept;

        /// @brief Executes pending asynchronous requests.
        void update() noexcept;

        /// @brief Prints a table of currently loaded bundles.
        void dbg_print_loaded_bundles(std::ostream os);

    private:
        struct _bundle_load_request
        {
            asset_bundle_id bundle;
        };

        // TODO: vvv this needs to be threadsafe vvv
        std::vector<_bundle_load_request> _bundle_load_requests;
        // TODO: vvv this needs to be threadsafe vvv
        std::vector<_bundle_load_request> _bundle_unload_requests;

        struct _available_bundles_table
        {
            std::vector<asset_bundle_id> guids;
            std::vector<std::string>     names;
            std::vector<std::ifstream>   sources;
        };
        _available_bundles_table _available_bundles;

        struct _loaded_bundles_table
        {
            std::vector<asset_bundle_id> guids;
            std::vector<asset_bundle>    bundles;
            std::vector<std::uint16_t>   refcount;
        };
        _loaded_bundles_table _loaded_bundles;

        struct _loaded_asset_table
        {
            std::vector<uuid>          guids;
            std::vector<std::uint16_t> refcount;
        };
        _loaded_asset_table _loaded_assets;

        void _increment_ref_count(const asset_id asset) noexcept;
        void _decrement_ref_count(const asset_id asset) noexcept;

        [[nodiscard]] std::tuple<bool, std::size_t>
        _find_loaded_asset_index(const asset_id) noexcept;

        [[nodiscard]] std::tuple<bool, std::size_t>
        _find_loaded_bundle_index(const asset_bundle_id) noexcept;

        void _load_bundle_metadata(std::ifstream&);

        void _unload_bundle_metadata();

        [[nodiscard]] std::vector<std::byte>
        _load_raw_asset(const asset_load_info& info) noexcept;

        void _execute_bundle_requests() noexcept;
    };

    template <typename T>
    T runtime_asset_manager::load_asset(const asset_bundle_id bundle, const uuid& id) noexcept
    {
        if (const auto [found, asset_index] = _find_loaded_asset_index(id); found)
        {
            ++_loaded_asset_refcount[asset_index];
            return;
        }

        if (const auto [found, bundle_index] = _find_loaded_bundle_index(bundle); found)
        {
            const auto& bundle = _loaded_bundles[bundle_index];

            if (const auto [asset_found, index] = find_asset_index(bundle, id); asset_found)
            {
                const auto& asset_load_info = bundle.asset_infos[index];
                const auto  bytes           = _load_raw_asset(asset_load_info);
                return T{ bytes };
            }
            else
            {
                DRAKO_LOG_ERROR("Can't locate asset in bundle");
                std::exit(EXIT_FAILURE);
            }
        }
        else
        {
            DRAKO_LOG_ERROR("Can't locate asset bundle with id " + std::to_string(bundle_id));
            std::exit(EXIT_FAILURE);
        }
    }

} // namespace drako::engine

#endif // !DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP