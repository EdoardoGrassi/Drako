#include <fstream>
#include <ios>
#include <iostream>

#include <vulkan/vulkan.hpp>

#include "drako/core/drako_api_win.hpp"
#include "drako/core/memory/unsync_linear_allocator.hpp"
#include "drako/core/system/native_window.hpp"

#include "drako/devel/asset_loader.hpp"
#include "drako/devel/asset_register.hpp"
#include "drako/devel/logging.hpp"

#include "drako/graphics/material.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/shader.hpp"
#include "drako/graphics/texture_types.hpp"
#include "drako/graphics/transform.hpp"

#include "drako/graphics/vulkan_forward_renderer.hpp"
#include "drako/graphics/vulkan_gpu_shader.hpp"
#include "drako/graphics/vulkan_memory_allocator.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_queue.hpp"
#include "drako/graphics/vulkan_render_system.hpp"
#include "drako/graphics/vulkan_resource_loader.hpp"
#include "drako/graphics/vulkan_swapchain.hpp"
#include "drako/graphics/vulkan_texture_types.hpp"
#include "drako/graphics/vulkan_wireframe_pipeline.hpp"

#include "drako/math/mat4x4.hpp"

#include "drako/plugins/wavefront_parser.hpp"

#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

using namespace drako;
namespace _fs = std::filesystem;

const _fs::path MESH_1_ASSET = "mesh_1.dkmesh";
const _fs::path VERT_SOURCE  = "gui_base.vert.spv";
const _fs::path FRAG_SOURCE  = "gui_base.frag.spv";

const std::vector<asset_desc> asset_descriptors = {};


using _g_alloc = unsync_linear_allocator;
_g_alloc g_allocator{ 1'000'000 };

std::vector<mesh_guid>      g_mesh_guids;
std::vector<mesh<_g_alloc>> g_mesh_assets;

std::vector<asset_guid>             g_mtl_template_guids;
std::vector<gpx::material_template> g_mtl_template_assets;

std::vector<asset_guid>             g_mtl_instance_guids;
std::vector<gpx::material_instance> g_mtl_instance_assets;

std::vector<texture_guid>      g_texture_guids;
std::vector<texture<_g_alloc>> g_texture_assets;

std::vector<asset_guid>         g_shader_guids;
std::vector<gpx::shader_source> g_shader_assets;

std::vector<asset_guid> g_asset_guids;
std::vector<asset_desc> g_asset_infos;

const std::vector<_fs::path> packages = {};
asset_loader<_g_alloc>       g_loader{ packages, 100'000, g_allocator };


void load(mesh_guid id) noexcept
{
    if (const auto guid = std::find(std::cbegin(g_mesh_guids), std::cend(g_mesh_guids), id);
        guid != std::cend(g_mesh_guids))
    { // asset already available in memory
        return;
    }
    else
    { // load from disk
        if (const auto found = std::find(std::cbegin(g_asset_guids), std::cend(g_asset_guids), id);
            found != std::cend(g_asset_guids))
        {
            const auto offset     = std::distance(std::cbegin(g_asset_guids), found);
            const auto descriptor = std::next(std::cbegin(g_asset_infos), offset);
            const auto mesh       = g_loader.load(*descriptor, g_allocator);
            g_mesh_guids.emplace_back(id);
            g_mesh_assets.emplace_back(mesh);
        }
        else
        {
            DRAKO_LOG_ERROR("[Drako] Asset descriptor of type <" DRAKO_STRINGIZE(mesh_guid) "> for ID <" + to_string(id) + "> not found");
            std::exit(EXIT_FAILURE);
        }
    }
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    sys::native_window main_window(L"Main window");

    main_window.show();
    mat4x4 m = transform(vec3(1, 1, 1), uquat::identity(), vec3(1));
    mat4x4 v = transform(vec3(0, 0, 0), uquat::identity(), vec3(1));
    mat4x4 p = ortographic(10, 10, 10, 10, 0, 1000);

    const auto instance = gpx::make_vulkan_instance();

    const auto [err_pdevice, pdevice] = gpx::make_vulkan_physical_device(instance.get());
    if (err_pdevice != vk::Result::eSuccess)
    {
        DRAKO_LOG_FAILURE("[Vulkan] " + to_string(err_pdevice));
        std::exit(EXIT_FAILURE);
    }

    const auto [err_surface, surface] = gpx::make_vulkan_surface(instance.get(), main_window);
    if (err_surface != vk::Result::eSuccess)
    {
        std::exit(EXIT_FAILURE);
    }

    const auto [err_device, device] = gpx::make_vulkan_logical_device(pdevice);
    if (err_device != vk::Result::eSuccess)
    {
        DRAKO_LOG_FAILURE("[Vulkan] " + to_string(err_device));
        std::exit(EXIT_FAILURE);
    }

    const gpx::shader_source vert_source = gpx::make_from_stream(std::ifstream(VERT_SOURCE, std::ios::binary));
    const gpx::shader_source frag_source = gpx::make_from_stream(std::ifstream(FRAG_SOURCE, std::ios::binary));

    const gpx::vulkan_gpu_shader vert(device.get(), vert_source);
    const gpx::vulkan_gpu_shader frag(device.get(), frag_source);

    const gpx::material_template mtl_template{};

    gpx::vulkan_forward_renderer renderer{ pdevice, device.get(), surface.get(), 1000, 1000 };
    gpx::vulkan_render_system    render_system{ device.get() };
    // gpx::vulkan_material_pipeline mtl_pipeline{ device.get(), rp, mtl_template };

    const auto wireframe_pipeline = gpx::make_vulkan_wireframe_pipeline(device.get(), vert, renderer.renderpass());
    render_system.create(1, wireframe_pipeline);

    const gpx::renderable_id renderables[] = { 1, 2, 3 };
    for (auto renderable : renderables)
    {
        render_system.istantiate(renderable, 1, 1);
    }

    const mat4x4 models[] = { translate(vec3(1, 1, 1)) };
    while (main_window.update())
    {
        const auto vp = v * p;

        std::vector<mat4x4> mvps(std::size(models));
        for (auto i = 0; i < std::size(models); ++i)
            mvps[i] = models[i] * vp;
    }

    /*
    MSG msg;
    do
    {
        while (PeekMessageW(&msg, main_window.window_handle_win32(), 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }
            // Translate keyboard virtual codes
            TranslateMessage(&msg);
            // Dispatch messages to custom handler
            DispatchMessageW(&msg);
        }

        // renderer.render();
    } while (msg.message != WM_QUIT);
    */
}