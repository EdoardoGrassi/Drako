#pragma once
#ifndef DRAKO_VULKAN_RENDER_SYSTEM_HPP
#define DRAKO_VULKAN_RENDER_SYSTEM_HPP

#include <algorithm>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "drako/graphics/material.hpp"
#include "drako/graphics/mesh_types.hpp"

#include "drako/graphics/vulkan_memory_allocator.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_texture_types.hpp"
#include "drako/graphics/vulkan_wireframe_pipeline.hpp"

namespace drako::gpx
{
    using pipeline_id   = std::uint32_t;
    using renderable_id = std::uint32_t;
    using mesh_id       = std::uint32_t;
    using texture_id    = std::uint32_t;
    using material_id   = std::uint32_t;

    class vulkan_render_system
    {
        using _vk_alloc     = vulkan_memory_allocator;
        using _vk_pipeline  = vulkan_graphics_pipeline;
        using _vk_mesh      = vulkan_mesh<_vk_alloc>;
        using _vk_text      = vulkan_texture<_vk_alloc>;
        using _vk_mesh_view = vulkan_mesh_view;
        using _vk_text_view = vulkan_texture_view;
        using _vk_mtl_data  = vulkan_material_instance;

        struct _rendereable_soa
        {
            std::vector<renderable_id> ids;
            std::vector<_vk_mesh_view> meshes;
            std::vector<_vk_mtl_data>  materials;
        };

    public:
        explicit vulkan_render_system(vk::Device device);

        void create(pipeline_id id, const vulkan_graphics_pipeline& p) noexcept;

        void create(mesh_id id, const mesh_view& m) noexcept;

        void create(texture_id id, const texture_view& t) noexcept;

        void create(material_id id, const material_template& mt) noexcept;

        void istantiate(renderable_id id, mesh_id mesh, material_id material) noexcept;

    private:
        const vk::Device _device;
        _vk_alloc        _allocator;

        std::vector<pipeline_id>  _pipeline_ids;
        std::vector<_vk_pipeline> _pipelines;

        std::vector<mesh_id>  _mesh_ids;
        std::vector<_vk_mesh> _meshes;

        std::vector<texture_id> _texture_ids;
        std::vector<_vk_text>   _textures;

        std::vector<material_id>       _material_ids;
        std::vector<material_template> _materials;

        _rendereable_soa _entities;
    };

    void vulkan_render_system::create(pipeline_id id, const vulkan_graphics_pipeline& p) noexcept
    {
        if (const auto found = std::find(std::cbegin(_pipeline_ids), std::cend(_pipeline_ids), id);
            found == std::cend(_pipeline_ids))
        {
            _pipeline_ids.emplace_back(id);
            _pipelines.emplace_back(p);
        }
    }

    void vulkan_render_system::create(mesh_id id, const mesh_view& m) noexcept
    {
        if (const auto found = std::find(std::cbegin(_mesh_ids), std::cend(_mesh_ids), id);
            found == std::cend(_mesh_ids))
        {
            _mesh_ids.emplace_back(id);
            _vk_mesh mesh{ _device, m, _allocator };
            _meshes.emplace_back(mesh);
        }
    }

    void vulkan_render_system::create(material_id id, const material_template& mt) noexcept
    {
        if (const auto found = std::find(std::cbegin(_material_ids), std::cend(_material_ids), id);
            found == std::cend(_material_ids))
        {
            _material_ids.emplace_back(id);
            _materials.emplace_back(mt);
        }
    }

    void vulkan_render_system::istantiate(renderable_id id, mesh_id mesh, material_id material) noexcept
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
        _entities.meshes.emplace_back(mesh_it);
        _entities.materials.emplace_back(mtl_it);
    }

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_RENDER_SYSTEM_HPP