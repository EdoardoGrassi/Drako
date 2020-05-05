#pragma once
#include "drako/graphics/render_system.hpp"

#include "drako/devel/logging.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"

#include <algorithm>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    const size_t DEVICE_ONLY_MEMORY_BYTES  = 10'000;
    const size_t HOST_VISIBLE_MEMORY_BYTES = 10'000;

    void render_system::_process_pending_meshes() noexcept;

    void render_system::_process_pending_entities() noexcept;

    render_system::render_system(const vulkan::context& ctx) noexcept
        : _device(ctx.logical_device.get())
        , _device_ubo_allocator(ctx, DEVICE_ONLY_MEMORY_BYTES)
        , _host_ubo_allocator(ctx, HOST_VISIBLE_MEMORY_BYTES)
        , _renderer(ctx.physical_device, ctx.logical_device.get(), ctx.surface.get(), 1000, 1000)
    {
    }

    void render_system::create(mesh_id id, const mesh_create_info& m) noexcept
    {
        DRAKO_ASSERT(is_valid(id));
        const _mesh_create_cmd cmd{ id, m };
        _create_meshes.push_back(cmd);
    }

    void render_system::create(shader_id id, const shader_create_info& s) noexcept
    {
        DRAKO_ASSERT(is_valid(id));
        const _shader_create_cmd cmd{ id, s };
        _create_shaders.push_back(cmd);
    }

    void render_system::destroy(mesh_id id) noexcept
    {
        DRAKO_ASSERT(is_valid(id));
        _destroy_meshes.push_back(id);
    }

    void render_system::destroy(shader_id id) noexcept
    {
        DRAKO_ASSERT(is_valid(id));
        _destroy_meshes.push_back(id);
    }

    void render_system::instantiate(renderable_id id, const renderable_create_info& info) noexcept
    {
        DRAKO_ASSERT(is_valid(id));
        _renderable_create_cmd cmd{ id, info };
        _create_renderables.push_back(cmd);
    }

    void render_system::update(
        const std::vector<renderable_id>& entities, const std::vector<mat4x4>& mvps) noexcept
    {
        _process_pending_meshes();
        // TODO: end impl


    }

} // namespace drako::gpx