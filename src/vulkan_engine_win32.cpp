#include "drako/core/memory/unsync_linear_allocator.hpp"
#include "drako/devel/asset_bundle_manifest.hpp"
#include "drako/engine/asset_manager.hpp"
#include "drako/engine/render_system.hpp"
#include "drako/graphics/transform.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_wireframe_pipeline.hpp"
#include "drako/system/desktop_window.hpp"

#include <vulkan/vulkan.hpp>

#include <iostream>

using namespace drako;
namespace _fs = std::filesystem;

const _fs::path MESH_1_ASSET = "mesh_1.dkmesh";
const _fs::path VERT_SOURCE  = "gui_base.vert.spv";
const _fs::path FRAG_SOURCE  = "gui_base.frag.spv";


using _g_alloc = unsync_linear_allocator;
_g_alloc g_allocator{ 1'000'000 };


int main()
{
    engine::AssetManager am{ {}, {} };

    const _fs::path path = "./bundles/main.dkbundle";
    const auto      size = static_cast<std::size_t>(_fs::file_size(path));
    auto            data = std::make_unique<std::byte[]>(size);

    {
        io::UniqueInputFile f{ path };
        f.read(data.get(), size);
    }

    AssetBundleManifestView main{ { data.get(), size } };
    // TODO: do things

    sys::desktop_window render_window(L"Drako Engine");
    render_window.show();

    mat4x4 m = transform(Vec3(1, 1, 1), quat{}, Vec3(1));
    mat4x4 v = transform(Vec3(0, 0, 0), quat{}, Vec3(1));
    mat4x4 p = ortographic(10, 10, 10, 10, 0, 1000);


    const vulkan::context context{ std::move(render_window) };
    vulkan::debug_print_queue_families(context);

    RenderSystem rs{ context };

    //const auto wireframe_pipeline = gpx::vulkan::make_wireframe_pipeline(context.logical_device.get(), vert, renderer.renderpass());
    //render_system.create(1, wireframe_pipeline);
    //render_system.create(1, vert_source);
    //render_system.create(2, frag_source);

    /*
    const render_id renderables[] = { 1, 2 };
    for (auto r : renderables)
        render_system.create(r, {});

    const mat4x4 models[] = { translate(vec3(1, 1, 1)) };

    std::vector<mat4x4> mvps(std::size(models));
    while (render_window.update())
    {
        const auto vp = v * p;
        for (auto i = 0; i < std::size(models); ++i)
            mvps[i] = models[i] * vp;

        //render_system.update();
    }
    */
}