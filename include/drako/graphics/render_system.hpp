#pragma once
#ifndef DRAKO_RENDER_SYSTEM_HPP_
#define DRAKO_RENDER_SYSTEM_HPP_

#include "drako/graphics/material_types.hpp"
#include "drako/graphics/mesh_types.hpp"

#include "drako/graphics/vulkan_forward_renderer.hpp"
#include "drako/graphics/vulkan_memory_allocator.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_texture_types.hpp"
#include "drako/graphics/vulkan_wireframe_pipeline.hpp"

#include <vector>

namespace drako::gpx
{
    using pipeline_id   = const std::uint32_t;
    using renderable_id = const std::uint32_t;
    using mesh_id       = const std::uint32_t;
    using texture_id    = const std::uint32_t;
    using material_id   = const std::uint32_t;
    using shader_id     = const std::uint32_t;

    [[nodiscard]] renderable_id new_renderable_id() noexcept
    {
        static std::remove_const_t<renderable_id> last_generated = 0;
        return ++last_generated;
    }

    class render_system
    {
        using _vk_alloc     = vulkan_memory_allocator;
        using _vk_pipeline  = vulkan_graphics_pipeline;
        using _vk_mesh      = vulkan_mesh<_vk_alloc>;
        using _vk_text      = vulkan_texture<_vk_alloc>;
        using _vk_mesh_view = vulkan_mesh_view;
        using _vk_text_view = vulkan_texture_view;
        using _vk_mtl_data  = vulkan_material_instance;
        using _vk_shader    = vulkan_gpu_shader;

        struct _rendereable_soa
        {
            std::vector<renderable_id> ids;
            std::vector<_vk_mesh_view> meshes;
            std::vector<_vk_mtl_data>  materials;
            std::vector<_vk_pipeline>  pipelines;
        };

    public:
        explicit render_system(const vulkan_runtime_context& ctx) noexcept;

        void create(pipeline_id id, const vulkan_graphics_pipeline& p) noexcept;

        void create(mesh_id id, const mesh_view& m) noexcept;

        void create(texture_id id, const texture_view& t) noexcept;

        void create(material_id id, const material_template& mt) noexcept;

        void create(shader_id id, const shader_source& s) noexcept;

        renderable_id instantiate(mesh_id mesh, material_id material) noexcept;
        void instantiate(renderable_id id, mesh_id mesh, material_id material) noexcept;

        void render(const std::vector<renderable_id>& entities, const std::vector<mat4x4>& mvps) noexcept;

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

        std::vector<shader_id>  _shaders_ids;
        std::vector<_vk_shader> _shaders;

        _rendereable_soa _entities;

        vulkan_forward_renderer _renderer;
    };

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_SYSTEM_HPP_