#pragma once
#ifndef DRAKO_BUILD_TYPES_HPP
#define DRAKO_BUILD_TYPES_HPP

#include <cstdint>
#include <vector>
#include <exception>
#include <filesystem>

namespace drako
{
    struct BuildJob
    {
        std::vector<std::filesystem::path> inputs;
        std::vector<std::filesystem::path> outputs;
        std::uint32_t                      id;
    };

    enum class BuildErrorCode
    {
    };

    struct BuildException : std::exception
    {
    };

} // namespace drako

#endif // !DRAKO_BUILD_TYPES_HPP