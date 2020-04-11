#pragma once
#ifndef DRAKO_MESH_IMPORTERS_HPP_
#define DRAKO_MESH_IMPORTERS_HPP_

#include "drako/devel/project_types.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/mesh_utils.hpp"

namespace drako::editor
{
    namespace _fs = std::filesystem;

    // import assets from .OBJ format files
    void import_obj_asset(const project_info& p, const _fs::path& src, const asset_import_context& ctx);

} // namespace drako::editor

#endif // !DRAKO_MESH_IMPORTERS_HPP_