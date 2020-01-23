#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "drako/devel/asset_register.hpp"

namespace _fs = std::filesystem;

const std::string PROGRAM_USAGE = "";

namespace drako
{
    using _build_asset_path = _fs::path;

    struct _build_item
    {
        std::uint32_t uuid;

        size_t             inputs_count;
        _build_asset_path* inputs;

        size_t             outputs_count;
        _build_asset_path* outputs;

        size_t transforms_count;
        void*  transforms;
    };

} // namespace drako

int main(int argc, char* argv[])
{
    for (auto i = 1; i < argc; ++i)
    {
        const _fs::path src{ argv[i] };
    }

    /*vvv single-threaded version vvv*/

    //const std::vector<_fs::path> metadata_files;
    //for (const auto& metadata : metadata_files) {}

    const std::vector<drako::_build_item> curr_build_items;
    std::vector<drako::_build_asset_path> curr_build_inputs;

    // TODO: load cache

    for (const auto& item : curr_build_items)
    {
    }
}