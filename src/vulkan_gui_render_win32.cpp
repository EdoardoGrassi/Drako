#include <fstream>
#include <ios>
#include <iostream>
#include <vulkan/vulkan.hpp>

#include "drako/core/drako_api_win.hpp"
#include "drako/core/memory/unsync_linear_allocator.hpp"
#include "drako/core/system/native_window.hpp"

#include "drako/graphics/shader_source.hpp"
#include "drako/graphics/vk_gui_renderer.hpp"
#include "drako/plugins/wavefront_mesh_loader.hpp"

using namespace drako;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    unsync_linear_allocator g_allocator(10'000);

    sys::native_window main_window(hInstance, L"Main window");

    //ShowWindow(main_window, nCmdShow);

    vk::Device ldevice;
    const auto vk_instance = gpx::create_vk_instance();
    const auto [vk_surface_result, vk_surface] = gpx::make_vulkan_surface(vk_instance.get(), main_window);
    if (vk_surface_result != vk::Result::eSuccess)
    {
        std::exit(EXIT_FAILURE);
    }

    gpx::shader_source vert = gpx::make_from_stream(std::ifstream("gui_base.vert.spv", std::ios::in | std::ios::binary));
    gpx::shader_source frag = gpx::make_from_stream(std::ifstream("gui_base.frag.spv", std::ios::in | std::ios::binary));

    const gpx::vulkan_gpu_shader vertex(ldevice, vert);
    const gpx::vulkan_gpu_shader fragment(ldevice, frag);

    gpx::vk_gui_renderer renderer(vk_instance.get(), vk_surface.get(), 0, vertex, fragment);

    MSG msg;
    do
    {
        while (PeekMessageW(&msg, main_window.native_handle(), 0, 0, PM_REMOVE))
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

        // Do other things...
    } while (msg.message != WM_QUIT);
}