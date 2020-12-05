#include "drako/engine/render_system.hpp"

#include "drako/devel/logging.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_staging_engine.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace drako
{
    using _this = RenderSystem;

    const std::size_t DEVICE_ONLY_MEMORY_BYTES  = 10'000;
    const std::size_t HOST_VISIBLE_MEMORY_BYTES = 10'000;

    void _this::_update_meshes() noexcept
    {
        // TODO: impl
    }

    void _this::_update_entities() noexcept
    {
        // TODO: impl
        for (const auto& e : _create_entities)
        {
            _entities.ids.push_back(e.id);
        }
        _entities.ids.clear();
    }

    void _this::_create_gpu_mesh(mesh_id id, const mesh& asset)
    {
        /*
        vulkan::mesh m{ _device, asset };

        const auto _device_v_buffer = _device_side_allocator.allocate(m.vertex_buffer_handle());
        const auto _device_i_buffer = _device_side_allocator.allocate(m.index_buffer_handle());

        const vulkan::memory_transfer info{};

        const auto callback = [this, id]() { _transferred_meshes.push_back(id); };
        _staging.submit(info, callback);
        */
        // TODO: impl
    }

    void _this::_destroy_gpu_mesh(mesh_id id)
    {
    }

    _this::RenderSystem(const vulkan::context& ctx) noexcept
        : _renderer(ctx)
    {
    }

    void _this::create(mesh_id id, const mesh_create_info& m) noexcept
    {
        assert(id);
        const _mesh_create_cmd cmd{ id, m };
        _create_meshes.push_back(cmd);
    }

    void _this::create(shader_id id, const shader_create_info& s) noexcept
    {
        assert(id);
        const _shader_create_cmd cmd{ id, s };
        _create_shaders.push_back(cmd);
    }

    void _this::destroy(mesh_id id) noexcept
    {
        assert(id);
        _destroy_meshes.push_back(id);
    }

    void _this::destroy(shader_id id) noexcept
    {
        assert(id);
        _destroy_shaders.push_back(id);
    }

    void _this::create(render_id id, const renderable_create_info& info) noexcept
    {
        assert(id);
        _renderable_create_cmd cmd{ id, info };
        _create_entities.push_back(cmd);
    }

    void _this::update(const frame_render_soa& data) noexcept
    {
        _update_meshes();
        _update_entities();

        // TODO: end impl
    }

} // namespace drako