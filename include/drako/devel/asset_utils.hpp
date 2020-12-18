#pragma once
#ifndef DRAKO_ASSET_UTILS_HPP
#define DRAKO_ASSET_UTILS_HPP

#include "drako/devel/asset_bundle_manifest.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/io/input_file_handle.hpp"
#include "drako/io/output_file_handle.hpp"

#include <limits>
#include <string>
#include <tuple>
#include <vector>

namespace drako
{
    /*
    void write_bundles_list(const std::vector<asset_bundle_meta>& bundles, std::ostream os)
    {
        using _bundle = std::remove_reference_t<decltype(bundles)>::value_type;
        static_assert(std::is_standard_layout_v<_bundle>,
            "Undefined behaviour on deserialization");
        static_assert(std::is_trivial_v<_bundle>,
            "Undefined behaviour on deserialization");

        std::ofstream ofs{ dst, std::ios::binary | std::ios::trunc };
        ofs.write(reinterpret_cast<const char*>(bundles.data()), std::size(bundles) * sizeof(_bundle));
    }
    */



    void write_bundle_metadata(const std::filesystem::path& dst);

    [[nodiscard]] inline std::vector<AssetBundleID>
    load_bundles_list(const std::filesystem::path& src)
    {
        const auto size   = static_cast<std::size_t>(std::filesystem::file_size(src));
        auto       memory = std::make_unique<std::byte[]>(size);

        std::span<std::byte> span{ memory.get(), size };
        {
            io::UniqueInputFile f{ src };
            io::read_exact(f, span);
        }

        const AssetBundleListView  v{ span };
        std::vector<AssetBundleID> result;
        result.resize(std::size(v.ids()));
        std::memcpy(std::data(result), std::data(v.ids()), std::size(v.ids()));

        return result;
    }

    /*
    [[nodiscard]] std::tuple<bool, std::size_t> find_asset_index(const asset_bundle& b, asset_id a)
    {
        if (const auto found = std::find(
                std::cbegin(b.asset_ids), std::cend(b.asset_ids), a);
            found != std::cend(b.asset_ids))
        {
            const auto index = std::distance(std::cbegin(b.asset_ids), found);
            return { true, index };
        }
        return { false, std::numeric_limits<size_t>::max() };
    }*/

    /*
    [[nodiscard]] std::tuple<bool, asset_file_info>
    find_info_in_bundle(const asset_bundle& b, asset_id id) noexcept
    {
        if (const auto found = std::find(
                std::cbegin(b.asset_ids), std::cend(b.asset_ids), id);
            found != std::cend(b.asset_ids))
        {
            const auto index = std::distance(std::cbegin(b.asset_ids), found);
            return { true, b.asset_infos[index] };
        }
        return { false, {} };
    }
    */

} // namespace drako

#endif // !DRAKO_ASSET_UTILS_HPP