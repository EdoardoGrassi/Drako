#include <fstream>
#include <ios>
#include <iostream>

#include <vulkan/vulkan.hpp>

#include "drako/core/memory/unsync_linear_allocator.hpp"
#include "drako/devel/logging.hpp"
#include "drako/devel/uuid.hpp"
#include "drako/engine/render_system.hpp"
#include "drako/engine/runtime_asset_manager.hpp"
#include "drako/graphics/material_types.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/shader_types.hpp"
#include "drako/graphics/transform.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_runtime_context.hpp"
#include "drako/graphics/vulkan_wireframe_pipeline.hpp"
#include "drako/system/desktop_window.hpp"

#if !defined(_drako_platform_Win32)
#error This source file should be included only on Windows builds
#endif

using namespace drako;
namespace _fs = std::filesystem;

const _fs::path MESH_1_ASSET = "mesh_1.dkmesh";
const _fs::path VERT_SOURCE  = "gui_base.vert.spv";
const _fs::path FRAG_SOURCE  = "gui_base.frag.spv";


using _g_alloc = unsync_linear_allocator;
_g_alloc g_allocator{ 1'000'000 };


const std::vector<asset_bundle_id> bundles_guids = { 1 };
const std::vector<std::string>     bundles_names = { "main" };
const std::vector<_fs::path>       bundles_paths = { "./bundles/main.dkbundle" };
const engine::bundle_manifest_soa  bundles{ bundles_guids, bundles_names, bundles_paths };
engine::runtime_asset_manager      g_asset_manager{ bundles, {} };


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const auto bundle = g_asset_manager.load_bundle("main");

    sys::desktop_window render_window(L"Drako engine");
    render_window.show();

    mat4x4 m = transform(vec3(1, 1, 1), uquat::identity(), vec3(1));
    mat4x4 v = transform(vec3(0, 0, 0), uquat::identity(), vec3(1));
    mat4x4 p = ortographic(10, 10, 10, 10, 0, 1000);

    const auto vert_source = gpx::make_from_stream(std::ifstream(VERT_SOURCE, std::ios::binary));
    const auto frag_source = gpx::make_from_stream(std::ifstream(FRAG_SOURCE, std::ios::binary));


    const gpx::vulkan::context context{ render_window };
    gpx::vulkan::debug_print_all_queue_families(context);

    render_system render_system{ context };

    //const auto wireframe_pipeline = gpx::vulkan::make_wireframe_pipeline(context.logical_device.get(), vert, renderer.renderpass());
    //render_system.create(1, wireframe_pipeline);
    //render_system.create(1, vert_source);
    //render_system.create(2, frag_source);

    const handle<renderable> renderables[] = { 1, 2 };
    for (auto r : renderables)
        render_system.instantiate(r, {});

    const mat4x4 models[] = { translate(vec3(1, 1, 1)) };

    std::vector<mat4x4> mvps(std::size(models));
    while (render_window.update())
    {
        const auto vp = v * p;
        for (auto i = 0; i < std::size(models); ++i)
            mvps[i] = models[i] * vp;

        //render_system.update();
    }
}