#pragma once
#ifndef DRAKO_VK_GUI_RENDERER_HPP
#define DRAKO_VK_GUI_RENDERER_HPP

#include "drako/devel/logging.hpp"
#include "drako/graphics/mesh_types.hpp"
#include "drako/graphics/vulkan_gpu_shader.hpp"

#include "drako/math/mat4x4.hpp"
#include "drako/math/vector2.hpp"
#include "drako/math/vector3.hpp"

#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    struct gui_vertex_data
    {
        vec2 position;
        vec3 color;

        [[nodiscard]] static const auto& vk_input_bindings() noexcept;

        [[nodiscard]] static const auto& vk_input_attributes() noexcept;
    };

    const auto& gui_vertex_data::vk_input_bindings() noexcept
    {
        const vk::VertexInputBindingDescription bindings[] = {
            { 0, sizeof(gui_vertex_data), vk::VertexInputRate::eVertex }
        };
        return bindings;
    }

    const auto& gui_vertex_data::vk_input_attributes() noexcept
    {
        const vk::VertexInputAttributeDescription attributes[] = {
            { 0, 0, vk::Format::eR32G32Sfloat, offsetof(gui_vertex_data, position) },
            { 0, 1, vk::Format::eR32G32B32Sfloat, offsetof(gui_vertex_data, color) }
        };
        return attributes;
    }


    // Executes rendering of GUI visual elements.
    class vk_gui_renderer
    {
    public:
        explicit vk_gui_renderer(vk::Instance instance, vk::SurfaceKHR surface,
            const uint32_t           queue_family_index,
            const vulkan_gpu_shader& vert,
            const vulkan_gpu_shader& frag) noexcept;

        ~vk_gui_renderer() noexcept;

        void record(vk::Rect2D render_area, vk::Viewport viewport, vk::Rect2D scissor) noexcept
        {
            const vk::ClearValue color_clear_value = std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f }; // full black
            const vk::ClearValue depth_clear_value = vk::ClearDepthStencilValue(1.0f, 0);
            const vk::ClearValue clear_values[]    = { color_clear_value, depth_clear_value };

            const vk::RenderPassBeginInfo renderpass_begin_info(
                _renderpass,
                _framebuffer,
                render_area,
                static_cast<uint32_t>(std::size(clear_values)), clear_values);

            // Start recording to the render pass instance
            _cmd_buffer.beginRenderPass(renderpass_begin_info, vk::SubpassContents::eInline);

            _cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);
            _cmd_buffer.setViewport(0, 1, &viewport);
            _cmd_buffer.setScissor(0, 1, &scissor);

            // TODO: drawing commands recording...

            // End of commands recording
            _cmd_buffer.endRenderPass();
        }

    private:
        vk::Instance       _instance;
        vk::PhysicalDevice _pdevice;
        vk::Device         _ldevice;

        vk::SurfaceKHR   _surface;
        vk::SwapchainKHR _swapchain;
        vk::Queue        _render_queue;  // executes drawing commands
        vk::Queue        _present_queue; // displays rendered images

        vk::CommandPool   _cmd_pool;
        vk::CommandBuffer _cmd_buffer;

        vk::DescriptorPool             _desc_pool;
        std::vector<vk::DescriptorSet> _desc_sets;

        vk::Semaphore _present_complete_sem;
        vk::Semaphore _drawing_complete_sem;

        vk::Pipeline   _pipeline;
        vk::RenderPass _renderpass;

        vk::Framebuffer _framebuffer;
        vk::Image       _color_buffer;
        vk::ImageView   _color_buffer_view;
        vk::Image       _depth_buffer;
        vk::ImageView   _depth_buffer_view;

        void setup_polygon_pipeline(const vulkan_gpu_shader& vert, const vulkan_gpu_shader& frag) noexcept;

        void setup_descriptors() noexcept;

        void setup_renderpass() noexcept;

        void setup_framebuffer(vk::RenderPass renderpass) noexcept;

        void setup_swapchain() noexcept;

        void render(vk::CommandBuffer* buffer) noexcept;
    };

} // namespace drako::gpx

#endif // !DRAKO_VK_GUI_RENDERER_HPP