#pragma once
#ifndef DRAKO_ASSET_UTILS_HPP_
#define DRAKO_ASSET_UTILS_HPP_

#include "drako/devel/asset_types.hpp"
#include "drako/io/input_file_stream.hpp"
#include "drako/io/output_file_stream.hpp"

#include <iostream>
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

    /*
    [[nodiscard]] load_bundles_result load_bundles_list(const std::filesystem::path& src)
    {
        namespace _fs = std::filesystem;
        using _header = asset_manifest_header;
        using _entry  = asset_manifest_entry;

        std::ifstream ifs{ src };
        const auto    file_size = _fs::file_size(src);

        auto local_temp_buffer = std::make_unique<std::byte[]>(file_size);
        ifs.read(reinterpret_cast<char*>(local_temp_buffer.get()), file_size);

        const auto header = reinterpret_cast<_header*>(local_temp_buffer.get());
        const auto items  = reinterpret_cast<_entry*>(local_temp_buffer.get() + sizeof(_header));
        for (auto i = 0; i < header->items_count; ++i)
        {
        }
    }*/

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

#endif // !DRAKO_ASSET_UTILS_HPP_