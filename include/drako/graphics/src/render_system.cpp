#pragma once
#include "drako/graphics/render_system.hpp"

#include "drako/devel/logging.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"

#include <algorithm>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    render_system::render_system(const vulkan_runtime_context& ctx) noexcept
        : _device(ctx.logical_device)
        , _allocator(ctx.physical_device, ctx.logical_device, {})
        , _renderer(ctx.physical_device, ctx.logical_device, ctx.surface, 1000, 1000)
    {
    }

    void render_system::create(pipeline_id id, const vulkan_graphics_pipeline& p) noexcept
    {
        if (const auto found = std::find(std::cbegin(_pipeline_ids), std::cend(_pipeline_ids), id);
            found == std::cend(_pipeline_ids))
        {
            _pipeline_ids.emplace_back(id);
            _pipelines.emplace_back(p);
        }
    }

    void render_system::create(mesh_id id, const mesh_view& m) noexcept
    {
        if (const auto found = std::find(std::cbegin(_mesh_ids), std::cend(_mesh_ids), id);
            found == std::cend(_mesh_ids))
        {
            _mesh_ids.emplace_back(id);
            _vk_mesh mesh{ _device, m, _allocator };
            _meshes.emplace_back(mesh);
        }
    }

    void render_system::create(material_id id, const material_template& mt) noexcept
    {
        if (const auto found = std::find(std::cbegin(_material_ids), std::cend(_material_ids), id);
            found == std::cend(_material_ids))
        {
            _material_ids.emplace_back(id);
            _materials.emplace_back(mt);
        }
    }

    void render_system::create(shader_id id, const shader_source& s) noexcept
    {
        _vk_shader vulkan_shader{ _device, s };
        _shaders_ids.emplace_back(id);
        _shaders.emplace_back(vulkan_shader);
    }

    renderable_id render_system::instantiate(mesh_id mesh, material_id material) noexcept
    {
        const auto id = new_renderable_id();

        const auto mesh_it = std::find(std::cbegin(_mesh_ids), std::cend(_mesh_ids), mesh);
        if (mesh_it == std::cend(_mesh_ids))
        {
            DRAKO_LOG_ERROR("[Drako] Mesh with ID <" + std::to_string(mesh) + "> not found.");
            return;
        }

        const auto mtl_it = std::find(std::cbegin(_material_ids), std::cend(_mesh_ids), material);
        if (mtl_it == std::cend(_mesh_ids))
        {
            DRAKO_LOG_ERROR("[Drako] Material with ID <" + std::to_string(material) + "> not found.");
            return;
        }

        _entities.ids.emplace_back(id);
        _entities.meshes.emplace_back(*mesh_it);
        _entities.materials.emplace_back(*mtl_it);
        return id;
    }

    void render_system::instantiate(renderable_id id, mesh_id mesh, material_id material) noexcept
    {
        if (std::find(std::cbegin(_entities.ids), std::cend(_entities.ids), id) != std::cend(_entities.ids))
        {
            DRAKO_LOG_ERROR("[Drako] An entity with ID <" + std::to_string(id) + "> alread exists.");
        }

        const auto mesh_it = std::find(std::cbegin(_mesh_ids), std::cend(_mesh_ids), mesh);
        if (mesh_it == std::cend(_mesh_ids))
        {
            DRAKO_LOG_ERROR("[Drako] Mesh with ID <" + std::to_string(mesh) + "> not found.");
            return;
        }

        const auto mtl_it = std::find(std::cbegin(_material_ids), std::cend(_mesh_ids), material);
        if (mtl_it == std::cend(_mesh_ids))
        {
            DRAKO_LOG_ERROR("[Drako] Material with ID <" + std::to_string(material) + "> not found.");
            return;
        }
        _entities.ids.emplace_back(id);
        _entities.meshes.emplace_back(*mesh_it);
        _entities.materials.emplace_back(*mtl_it);
    }

    void render_system::render(
        const std::vector<renderable_id>& entities,
        const std::vector<mat4x4>&        mvps) noexcept
    {
        // TODO: end impl
        for (const auto& p : _pipelines)
        {
            _renderer.draw(p, )
        }
    }

} // namespace drako::gpx