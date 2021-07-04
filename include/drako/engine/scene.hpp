#pragma once
#ifndef DRAKO_SCENE_HPP
#define DRAKO_SCENE_HPP

#include "drako/devel/asset_types.hpp"
#include "drako/engine/asset_system.hpp"
#include "drako/graphics/mesh_types.hpp"

#include <vector>

namespace drako
{
    struct Scene
    {
        std::vector<AssetID> meshes;
    };

    void load_scene(const engine::AssetSystemRuntime& a, const Scene& s);

} // namespace drako

#endif // !DRAKO_SCENE_HPP