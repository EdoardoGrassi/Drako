#pragma once
#ifndef DRAKO_RENDER_SYSTEM_HPP
#define DRAKO_RENDER_SYSTEM_HPP

#include "drako/graphics/material_types.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/vulkan_forward_renderer.hpp"
#include "drako/graphics/vulkan_memory_allocator.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_staging_allocator.hpp"
#include "drako/graphics/vulkan_texture_types.hpp"
#include "drako/graphics/vulkan_wireframe_pipeline.hpp"

#include <vector>

namespace drako::gpx
{
    using pipeline_id   = std::uint32_t;
    using renderable_id = std::uint32_t;
    using mesh_id       = std::uint32_t;
    using texture_id    = std::uint32_t;
    using material_id   = std::uint32_t;
    using shader_id     = std::uint32_t;


    struct mesh_create_info
    {
    };

    struct shader_create_info
    {
    };

    struct renderable_create_info
    {
    };

    [[nodiscard]] mesh_id new_mesh_id() noexcept
    {
        static mesh_id last_gen = 0;
        return ++last_gen;
    }

    [[nodiscard]] constexpr bool is_valid(mesh_id id) noexcept
    {
        return id == 0;
    }

    [[nodiscard]] renderable_id new_renderable_id() noexcept
    {
        static renderable_id last_generated = 0;
        return ++last_generated;
    }

    class render_system
    {
        using _vk_pipeline  = vulkan::graphics_pipeline;
        using _vk_mesh_view = vulkan::mesh_view;
        using _vk_text_view = vulkan::texture_view;
        using _vk_mtl_data  = vulkan::material_instance;
        using _vk_shader    = vulkan::gpu_shader;

    public:
        explicit render_system(const vulkan::context& ctx) noexcept;

        //void create(pipeline_id, const vulkan::graphics_pipeline&) noexcept;
        void create(mesh_id, const mesh_create_info&) noexcept;
        void create(texture_id, const texture_view&) noexcept;
        void create(material_id, const material_template&) noexcept;
        void create(shader_id, const shader_create_info&) noexcept;

        void instantiate(renderable_id, const renderable_create_info&) noexcept;

        void destroy(mesh_id) noexcept;
        void destroy(shader_id) noexcept;

        void update(const std::vector<renderable_id>& entities, const std::vector<mat4x4>& mvps) noexcept;

    private:
        template <typename TID, typename TInfo>
        struct _basic_cmd
        {
            TInfo info;
            TID   uuid;

            explicit _basic_cmd(TID _uuid, const TInfo& _info) noexcept
                : info{ _info }, uuid{ _uuid }
            {
            }
        };

        using _mesh_create_cmd       = _basic_cmd<mesh_id, mesh_create_info>;
        using _shader_create_cmd     = _basic_cmd<shader_id, shader_create_info>;
        using _renderable_create_cmd = _basic_cmd<renderable_id, renderable_create_info>;


        const vk::Device               _device;
        vulkan::device_local_allocator _device_ubo_allocator;
        vulkan::host_mapped_allocator  _host_ubo_allocator;
        vulkan::staging_allocator      _resource_loader;

        vulkan::fixed_device_ubo _vertex_buffer;
        vulkan::fixed_device_ubo _index_buffer;
        vulkan::fixed_host_ubo   _transforms_buffer;

        /* scheduled resource allocations */

        std::vector<_mesh_create_cmd>       _create_meshes;
        std::vector<_shader_create_cmd>     _create_shaders;
        std::vector<_renderable_create_cmd> _create_renderables;

        /* scheduled resource deallocations */

        std::vector<mesh_id>       _destroy_meshes;
        std::vector<shader_id>     _destroy_shaders;
        std::vector<renderable_id> _destroy_renderables;

        struct _renderable_table
        {
            std::vector<renderable_id> ids;
            std::vector<_vk_mesh_view> meshes;
            std::vector<_vk_mtl_data>  materials;
            std::vector<_vk_pipeline>  pipelines;
        };
        _renderable_table _renderables;

        vulkan::forward_renderer _renderer;

        void _process_pending_meshes() noexcept;

        void _process_pending_entities() noexcept;
    };

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_SYSTEM_HPP