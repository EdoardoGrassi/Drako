#pragma once
#ifndef DRAKO_RENDER_SYSTEM_HPP
#define DRAKO_RENDER_SYSTEM_HPP

#include "drako/core/typed_handle.hpp"
#include "drako/graphics/material_types.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/vulkan_forward_renderer.hpp"
#include "drako/graphics/vulkan_memory_allocator.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_staging_engine.hpp"
#include "drako/graphics/vulkan_texture_types.hpp"
#include "drako/graphics/vulkan_wireframe_pipeline.hpp"

#include <vector>

namespace drako
{
    using pipeline_id = std::uint32_t;
    using texture_id  = std::uint32_t;
    using material_id = std::uint32_t;
    using shader_id   = std::uint32_t;



    struct renderable
    {
    };


    struct mesh_create_info
    {
        handle<mesh> id;
        mesh         data;
    };

    struct shader_create_info
    {
    };

    struct renderable_create_info
    {
        handle<renderable> entity;
        handle<mesh>       mesh;
        // TODO: add other fields
    };



    class render_system
    {
        using _pipeline = gpx::vulkan::graphics_pipeline;
        using _mesh     = gpx::vulkan::mesh;
        using _texture  = gpx::vulkan::texture_view;
        using _mtl_data = gpx::vulkan::material_instance;
        using _shader   = gpx::vulkan::gpu_shader;

    public:
        // contains data for the rendering of a single frame
        struct frame_render_soa
        {
            std::vector<handle<renderable>> entities;
            std::vector<mat4x4>             mvps;
        };

        explicit render_system(const gpx::vulkan::context& ctx) noexcept;

        //void create(pipeline_id, const vulkan::graphics_pipeline&) noexcept;
        void create(handle<mesh>, const mesh_create_info&) noexcept;
        void create(texture_id, const texture_view&) noexcept;
        void create(material_id, const gpx::material_template&) noexcept;
        void create(shader_id, const shader_create_info&) noexcept;

        void instantiate(handle<renderable>, const renderable_create_info&) noexcept;

        void destroy(handle<mesh>) noexcept;
        void destroy(shader_id) noexcept;

        void update(const frame_render_soa&) noexcept;

    private:
        template <typename THandle, typename TInfo>
        struct _basic_cmd
        {
            TInfo   info;
            THandle handle;

            explicit _basic_cmd(THandle _handle, const TInfo& _info) noexcept
                : info{ _info }, handle{ _handle } {}
        };

        using _mesh_create_cmd       = _basic_cmd<handle<mesh>, mesh_create_info>;
        using _shader_create_cmd     = _basic_cmd<shader_id, shader_create_info>;
        using _renderable_create_cmd = _basic_cmd<handle<renderable>, renderable_create_info>;


        const vk::Device                    _device;
        gpx::vulkan::device_local_allocator _device_side_allocator;
        gpx::vulkan::host_mapped_allocator  _host_side_allocator;
        gpx::vulkan::staging_engine         _staging;

        gpx::vulkan::fixed_device_ubo _vertex_buffer;
        gpx::vulkan::fixed_device_ubo _index_buffer;
        gpx::vulkan::fixed_host_ubo   _transforms_buffer;

        /* currently available resources */

        struct _avail_mesh_soa
        {
            std::vector<handle<mesh>>      id;
            std::vector<gpx::vulkan::mesh> meshes;
        } meshes;

        /* resources scheduled for construction */

        std::vector<_mesh_create_cmd>       _create_meshes;   // TODO: needs to be threadsafe
        std::vector<_shader_create_cmd>     _create_shaders;  // TODO: needs to be threadsafe
        std::vector<_renderable_create_cmd> _create_entities; // TODO: needs to be threadsafe

        /* resources that completed transfert on gpu */

        std::vector<handle<mesh>> _transferred_meshes; // TODO: needs to be threadsafe

        /* resources scheduled for destruction */

        std::vector<handle<mesh>>       _destroy_meshes;   // TODO: needs to be threadsafe
        std::vector<shader_id>          _destroy_shaders;  // TODO: needs to be threadsafe
        std::vector<handle<renderable>> _destroy_entities; // TODO: needs to be threadsafe

        struct _renderable_table
        {
            std::vector<handle<renderable>> ids;
            std::vector<handle<mesh>>       meshes;
            std::vector<handle<_mtl_data>>  materials;
            std::vector<handle<_pipeline>>  pipelines;
        };
        _renderable_table _entities;

        gpx::vulkan::forward_renderer _renderer;

        void _update_meshes() noexcept;

        void _update_entities() noexcept;

        void _create_gpu_mesh(handle<mesh>, const mesh&);
        void _destroy_gpu_mesh(handle<mesh>);
    };

} // namespace drako

#endif // !DRAKO_RENDER_SYSTEM_HPP