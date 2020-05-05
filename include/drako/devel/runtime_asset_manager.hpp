#pragma once
#ifndef DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP_
#define DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP_

#include "drako/devel/assertion.hpp"
#include "drako/devel/asset_loader.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/asset_utils.hpp"

#include <vector>

namespace drako::engine
{

    //template <typename ...Assets>
    class runtime_asset_manager
    {
    public:
        // explicit runtime_asset_manager(const std::vector<asset_bundle>& bundles);

        explicit runtime_asset_manager(
            const std::vector<asset_bundle_id>& bundle_guids,
            const std::vector<std::string>&     bundle_names,
            const std::vector<_fs::path>&       bundle_paths);

        runtime_asset_manager(const runtime_asset_manager&) = delete;
        runtime_asset_manager& operator=(const runtime_asset_manager&) = delete;

        runtime_asset_manager(runtime_asset_manager&&) = delete;
        runtime_asset_manager& operator=(runtime_asset_manager&&) = delete;


        [[nodiscard]] const asset_bundle& load_bundle(const asset_bundle_id id) noexcept;
        [[nodiscard]] const asset_bundle& load_bundle(const std::string_view name) noexcept;
        void                              load_bundle(const std::vector<asset_bundle_id>& ids) noexcept;
        void                              load_bundle(const std::vector<std::string_view>& names) noexcept;

        void load_bundle_async(const asset_bundle_id id) noexcept;

        void unload_bundle(const asset_bundle_id id) noexcept;
        void unload_bundle(const std::vector<asset_bundle_id>& ids) noexcept;

        template <typename T>
        [[nodiscard]] T load_asset(const asset_bundle_id bundle, const uuid& id) noexcept;

        void load_asset(const uuid& id) noexcept;
        void load_asset(const std::vector<uuid>& ids) noexcept;

        void load_asset_async() noexcept;

    private:
        struct _bundle_load_request
        {
            asset_bundle_id bundle;
        };

        asset_loader _loader;

        // TODO: vvv this needs to be threadsafe vvv
        std::vector<_bundle_load_request> _bundle_load_requests;
        // TODO: vvv this needs to be threadsafe vvv
        std::vector<_bundle_load_request> _bundle_unload_requests;

        std::vector<asset_bundle_id> _available_bundles_guids;
        std::vector<std::string>     _available_bundles_names;
        std::vector<std::ifstream>   _available_bundles_sources;

        std::vector<asset_bundle_id> _loaded_bundles_guids;
        std::vector<asset_bundle>    _loaded_bundles;
        std::vector<std::uint16_t>   _loaded_bundles_refcount;

        std::vector<uuid>          _loaded_asset_guids;
        std::vector<std::uint16_t> _loaded_asset_refcount;

        void _increment_ref_count(const asset_id asset) noexcept;
        void _decrement_ref_count(const asset_id asset) noexcept;

        [[nodiscard]] std::tuple<bool, std::size_t>
        _find_loaded_asset_index(const asset_id id) noexcept;

        [[nodiscard]] std::tuple<bool, std::size_t>
        _find_loaded_bundle_index(const asset_bundle_id id) noexcept;

        [[nodiscard]] std::vector<std::byte>
        _load_raw_asset(const asset_load_info& info) noexcept;

        void _execute_pending_bundle_requests() noexcept;
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

#endif // !DRAKO_ENGINE_RUNTIME_ASSET_MANAGER_HPP_