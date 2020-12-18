#include "drako/engine/asset_manager.hpp"
#include "drako/engine/render_system.hpp"
#include "drako/graphics/transform.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_wireframe_pipeline.hpp"
#include "drako/system/desktop_window.hpp"

#include <iostream>

using namespace drako;


int main()
{
    engine::AssetManager am{ {}, {} };

    sys::UniqueDesktopWindow render_window(L"Drako Engine");
    render_window.show();

    Mat4x4 m = transform(Vec3(1, 1, 1), Quat{}, Vec3(1));
    Mat4x4 v = transform(Vec3(0, 0, 0), Quat{}, Vec3(1));
    Mat4x4 p = ortographic(10, 10, 10, 10, 0, 1000);


    const vulkan::Context vctx{ std::move(render_window) };
    vulkan::debug_print_queue_families(vctx);

    RenderSystem rs{ vctx };

    //const auto wireframe_pipeline = gpx::vulkan::make_wireframe_pipeline(context.logical_device.get(), vert, renderer.renderpass());
    //render_system.create(1, wireframe_pipeline);
    //render_system.create(1, vert_source);
    //render_system.create(2, frag_source);

    /*
    const render_id renderables[] = { 1, 2 };
    for (auto r : renderables)
        render_system.create(r, {});

    const Mat4x4 models[] = { translate(vec3(1, 1, 1)) };

    std::vector<Mat4x4> mvps(std::size(models));
    while (render_window.update())
    {
        const auto vp = v * p;
        for (auto i = 0; i < std::size(models); ++i)
            mvps[i] = models[i] * vp;

        //render_system.update();
    }
    */
}