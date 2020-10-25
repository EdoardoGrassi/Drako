#pragma once
#ifndef DRAKO_BUILD_TYPES_HPP_
#define DRAKO_BUILD_TYPES_HPP_

#include <cstdint>
#include <exception>
#include <filesystem>

namespace drako
{
    namespace _fs = std::filesystem;


    struct project_build_item
    {
        std::uint32_t id;

        size_t    inputs_count;
        _fs::path inputs;

        size_t     outputs_count;
        _fs::path* outputs;

        size_t transforms_count;
        void*  transforms;
    };


    enum class build_error_code
    {
    };

    struct build_error : std::exception
    {
    };

    struct packaging_result
    {
    };

} // namespace drako

#endif // !DRAKO_BUILD_TYPES_HPP_