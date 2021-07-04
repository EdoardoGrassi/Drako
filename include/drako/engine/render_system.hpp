#pragma once
#ifndef DRAKO_RENDER_SYSTEM_HPP
#define DRAKO_RENDER_SYSTEM_HPP

#include "drako/core/typed_handle.hpp"
#include "drako/graphics/material_types.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/texture_types.hpp"
#include "drako/graphics/vulkan_render_engine.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/math/mat4x4.hpp"

#include <glm/mat4x4.hpp>

#include <vector>

namespace drako
{
    DRAKO_DEFINE_TYPED_ID(pipeline_id, std::uint32_t);
    DRAKO_DEFINE_TYPED_ID(texture_id, std::uint32_t);
    DRAKO_DEFINE_TYPED_ID(material_id, std::uint32_t);
    DRAKO_DEFINE_TYPED_ID(mesh_id, std::uint32_t);
    DRAKO_DEFINE_TYPED_ID(shader_id, std::uint32_t);
    DRAKO_DEFINE_TYPED_ID(render_id, std::uint32_t);


    struct mesh_create_info
    {
        mesh data;
    };

    struct shader_create_info
    {
    };

    struct renderable_create_info
    {
        render_id entity;
        mesh_id   mesh;
        // TODO: add other fields
    };



    class RenderSystem
    {
    public:
        // contains data for the rendering of a single frame
        struct frame_render_soa
        {
            std::vector<render_id> entities;
            std::vector<glm::mat4>    mvps;
        };

        explicit RenderSystem(const vulkan::Context& ctx) noexcept;

        //void create(pipeline_id, const vulkan::graphics_pipeline&) noexcept;
        void create(mesh_id, const mesh_create_info&) noexcept;
        //void create(texture_id, const texture_view&) noexcept;
        void create(material_id, const material_template&) noexcept;
        void create(shader_id, const shader_create_info&) noexcept;
        void create(render_id, const renderable_create_info&) noexcept;

        void destroy(mesh_id) noexcept;
        void destroy(shader_id) noexcept;

        void update(const frame_render_soa&) noexcept;

    private:
        template <typename TID, typename TInfo>
        struct _basic_cmd
        {
            TInfo info;
            TID   id;

            explicit _basic_cmd(TID _id, const TInfo& _info) noexcept
                : info{ _info }, id{ _id } {}
        };

        using _mesh_create_cmd       = _basic_cmd<mesh_id, mesh_create_info>;
        using _shader_create_cmd     = _basic_cmd<shader_id, shader_create_info>;
        using _renderable_create_cmd = _basic_cmd<render_id, renderable_create_info>;

        /* currently available resources */

        struct _avail_mesh_soa
        {
            std::vector<mesh_id> id;
            std::vector<mesh>    meshes;
        } meshes;

        /* resources scheduled for construction */

        std::vector<_mesh_create_cmd>       _create_meshes;   // TODO: needs to be threadsafe
        std::vector<_shader_create_cmd>     _create_shaders;  // TODO: needs to be threadsafe
        std::vector<_renderable_create_cmd> _create_entities; // TODO: needs to be threadsafe

        /* resources that completed transfert on gpu */

        std::vector<mesh_id> _transferred_meshes; // TODO: needs to be threadsafe

        /* resources scheduled for destruction */

        std::vector<mesh_id>   _destroy_meshes;   // TODO: needs to be threadsafe
        std::vector<shader_id> _destroy_shaders;  // TODO: needs to be threadsafe
        std::vector<render_id> _destroy_entities; // TODO: needs to be threadsafe

        struct _renderable_table
        {
            std::vector<render_id>   ids;
            std::vector<mesh_id>     meshes;
            std::vector<material_id> materials;
            std::vector<pipeline_id> pipelines;
        };
        _renderable_table _entities;

        vulkan::RenderEngine _renderer;

        void _update_meshes() noexcept;

        void _update_entities() noexcept;

        void _create_gpu_mesh(mesh_id, const mesh&);
        void _destroy_gpu_mesh(mesh_id);
    };

} // namespace drako

#endif // !DRAKO_RENDER_SYSTEM_HPP