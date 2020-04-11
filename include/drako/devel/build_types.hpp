#pragma once
#ifndef DRAKO_BUILD_TYPES_HPP_
#define DRAKO_BUILD_TYPES_HPP_

#include <cstdint>
#include <filesystem>

namespace drako
{
    namespace _fs           = std::filesystem;
    using _build_asset_path = std::filesystem::path;


    struct project_build_item
    {
        std::uint32_t uuid;

        size_t             inputs_count;
        _build_asset_path* inputs;

        size_t             outputs_count;
        _build_asset_path* outputs;

        size_t transforms_count;
        void*  transforms;
    };


    enum class build_error_code
    {
    };

    struct build_error
    {
        build_error_code ec;
    };

    struct packaging_result
    {
        std::vector<asset_package_descriptor>   packages;
        std::vector<asset_packaging_descriptor> assets;
    };

} // namespace drako

#endif // !DRAKO_BUILD_TYPES_HPP_