#pragma once
#ifndef DRAKO_BUILD_UTILS_HPP
#define DRAKO_BUILD_UTILS_HPP

#include "drako/devel/asset_types.hpp"
#include "drako/devel/build_types.hpp"

namespace drako
{
    [[nodiscard]] AssetManifestRecord build_asset_descriptor(const Uuid& id);

    //[[nodiscard]] std::variant<asset_manifest, build_error>
    //build_asset_manifest(const fs::path& build_folder);

} // namespace drako

#endif // !DRAKO_BUILD_UTILS_HPP