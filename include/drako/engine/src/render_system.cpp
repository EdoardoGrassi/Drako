#include "drako/engine/render_system.hpp"

#include "drako/devel/logging.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_staging_engine.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace drako
{
    using _this = render_system;

    const size_t DEVICE_ONLY_MEMORY_BYTES  = 10'000;
    const size_t HOST_VISIBLE_MEMORY_BYTES = 10'000;

    void _this::_update_meshes() noexcept
    {
        // TODO: impl
    }

    void _this::_update_entities() noexcept
    {
        // TODO: impl
        for (auto e : _create_entities)
        {
            _entities.ids.push_back(e.entity);
        }
        _entities.ids.clear();
    }

    void _this::_create_gpu_mesh(handle<mesh> id, const mesh& asset)
    {
        gpx::vulkan::mesh m{ _device, asset };

        const auto _host_v_buffer = _host_side_allocator.allocate(m.vertex_buffer_handle());
        const auto _host_i_buffer = _host_side_allocator.allocate(m.index_buffer_handle());
        std::memcpy(_host_v_buffer, asset.vertex_buffer().data(), asset.vertex_buffer().size_bytes());
        std::memcpy(_host_i_buffer, asset.index_buffer().data(), asset.index_buffer().size_bytes());

        const auto _device_v_buffer = _device_side_allocator.allocate(m.vertex_buffer_handle());
        const auto _device_i_buffer = _device_side_allocator.allocate(m.index_buffer_handle());

        const gpx::vulkan::memory_transfer_info info{};

        const auto callback = [this, id]() { _transferred_meshes.push_back(id); };
        _staging.submit_for_transfer(info, callback);
    }

    void _this::_destroy_gpu_mesh(handle<mesh> id)
    {
    }

    _this::render_system(const gpx::vulkan::context& ctx) noexcept
        : _device(ctx.logical_device.get())
        , _device_ubo_allocator(ctx, DEVICE_ONLY_MEMORY_BYTES)
        , _host_ubo_allocator(ctx, HOST_VISIBLE_MEMORY_BYTES)
        , _renderer(ctx, 1000, 1000)
    {
    }

    void _this::create(handle<mesh> id, const mesh_create_info& m) noexcept
    {
        assert(is_valid(id));
        const _mesh_create_cmd cmd{ id, m };
        _create_meshes.push_back(cmd);
    }

    void _this::create(shader_id id, const shader_create_info& s) noexcept
    {
        assert(is_valid(id));
        const _shader_create_cmd cmd{ id, s };
        _create_shaders.push_back(cmd);
    }

    void _this::destroy(handle<mesh> id) noexcept
    {
        assert(is_valid(id));
        _destroy_meshes.push_back(id);
    }

    void _this::destroy(shader_id id) noexcept
    {
        assert(is_valid(id));
        _destroy_meshes.push_back(id);
    }

    void _this::instantiate(handle<renderable> id, const renderable_create_info& info) noexcept
    {
        assert(is_valid(id));
        _renderable_create_cmd cmd{ id, info };
        _create_renderables.push_back(cmd);
    }

    void _this::update(const frame_render_soa& data) noexcept
    {
        _update_meshes();
        _update_entities();

        // TODO: end impl
    }

} // namespace drako