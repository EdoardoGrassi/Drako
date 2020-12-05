#pragma once
#ifndef DRAKO_BUILD_UTILS_HPP_
#define DRAKO_BUILD_UTILS_HPP_

#include "drako/devel/asset_types.hpp"
#include "drako/devel/build_types.hpp"

#include <filesystem>
#include <string>
#include <variant>
#include <vector>

namespace fs = std::filesystem; // convenient alias

namespace drako
{
    /// @brief Cyclic redundancy check (version C32) of given byte sequence.
    [[nodiscard]] std::uint32_t crc32c(const std::byte* data, std::size_t size) noexcept;


    /// @brief Cyclic redundancy check (version C32) of given byte sequence.
    [[nodiscard]] std::uint32_t crc32c(const std::uint32_t* data, std::size_t size) noexcept;


    /// @brief Cyclic redundancy check (version C32) of given byte sequence.
    [[nodiscard]] std::uint32_t crc32c(const std::uint64_t* data, std::size_t size) noexcept;


    [[nodiscard]] asset_manifest_entry
    build_asset_descriptor(const Uuid& id);


    [[nodiscard]] std::variant<packaging_result, build_error>
    build_packages_for_devel(const fs::path& folder, const std::vector<std::string>& names, const std::vector<asset_type>& types);


    //[[nodiscard]] std::variant<asset_manifest, build_error>
    //build_asset_manifest(const fs::path& build_folder);

} // namespace drako

#endif // !DRAKO_BUILD_UTILS_HPP_