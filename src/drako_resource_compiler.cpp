#pragma once

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "drako/devel/build_utils.hpp"

namespace _fs = std::filesystem;

const std::string PROGRAM_USAGE = "";

int main(int argc, char* argv[])
{
    for (auto i = 1; i < argc; ++i)
    {
        const _fs::path src{ argv[i] };
    }

    /*vvv single-threaded version vvv*/

    //const std::vector<_fs::path> metadata_files;
    //for (const auto& metadata : metadata_files) {}

    const std::vector<drako::project_build_item> curr_build_items;
    std::vector<_fs::path> curr_build_inputs;

    // TODO: load cache

    for (const auto& item : curr_build_items)
    {
    }
}