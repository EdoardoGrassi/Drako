#pragma once
#ifndef DRAKO_ASSET_UTILS_HPP_
#define DRAKO_ASSET_UTILS_HPP_

#include "drako/devel/asset_types.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace drako
{
    namespace sfs = std::filesystem;

    struct load_bundles_result
    {
        std::vector<asset_bundle_id>       ids;
        std::vector<asset_manifest_header> bundles;
        std::vector<std::string>           names;
    };

    [[nodiscard]] load_bundles_result load_bundles_list(const sfs::path& file)
    {
        using _man_header = asset_manifest_header;
        using _man_entry  = asset_manifest_entry;

        std::ifstream ifs{ file };
        const auto    file_size = sfs::file_size(file);

        std::vector<std::byte> local_temp_buffer(file_size);
        ifs.read(reinterpret_cast<char*>(local_temp_buffer.data()), file_size);

        const auto header = reinterpret_cast<_man_header*>(local_temp_buffer.data());
        const auto items  = reinterpret_cast<_man_entry*>(local_temp_buffer.data() + sizeof(_man_header));
        for (auto i = 0; i < header->items_count; ++i)
        {
        }
    }


    struct asset_bundle
    {
        std::vector<asset_id>        asset_ids;
        std::vector<asset_file_info> asset_infos;
    };

    [[nodiscard]] asset_bundle load_bundle_manifest(const sfs::path& file)
    {
        using _manif_header = asset_manifest_header;
        using _manif_entry  = asset_manifest_entry;

        std::ifstream ifs{ file };

        _manif_header header;
        if (!ifs.read(reinterpret_cast<char*>(&header), sizeof(_manif_header)))
            std::exit(EXIT_FAILURE);

        asset_bundle result;

        return result;
    }

} // namespace drako

#endif // !DRAKO_ASSET_UTILS_HPP_