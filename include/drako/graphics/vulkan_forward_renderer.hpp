#pragma once
#ifndef DRAKO_VULKAN_FORWARD_RENDERER_HPP
#define DRAKO_VULKAN_FORWARD_RENDERER_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/graphics/vulkan_material_pipeline.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/math/mat4x4.hpp"

#include <vector>

#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    class vulkan_forward_renderer
    {
    public:
        explicit vulkan_forward_renderer(
            vk::PhysicalDevice pdevice,
            vk::Device         device,
            vk::SurfaceKHR     surface,
            uint32_t           width,
            uint32_t           height) noexcept;
        ~vulkan_forward_renderer() noexcept;

        vulkan_forward_renderer(const vulkan_forward_renderer&) = delete;
        vulkan_forward_renderer& operator=(const vulkan_forward_renderer&) = delete;

        vulkan_forward_renderer(vulkan_forward_renderer&&) = delete;
        vulkan_forward_renderer& operator=(vulkan_forward_renderer&&) = delete;

        DRAKO_NODISCARD
        vk::RenderPass renderpass() const noexcept
        {
            return _renderpass.get();
        }

        void render(const vulkan_material_pipeline&      pipeline,
            const std::vector<mat4x4>&                   mvps,
            const std::vector<vulkan_mesh_view>&         meshes,
            const std::vector<vulkan_material_instance>& materials) noexcept;

    private:
        struct frame_attachments
        {
            vk::Framebuffer framebuffer;
            // vk::Image       color_buffer; << provided by the swapchain
            vk::ImageView color_buffer_view;
            vk::Image     depth_buffer;
            vk::ImageView depth_buffer_view;
        };

        void setup_descriptors() noexcept;

        void setup_renderpass() noexcept;

        void setup_swapchain(vk::PhysicalDevice pdevice) noexcept;

        vk::Device _ldevice;

        vk::Rect2D             _render_area;
        vk::SurfaceKHR         _surface;
        vk::UniqueSwapchainKHR _swapchain;
        std::vector<vk::Image> _swapchain_images;

        vk::Queue     _command_queue;        // executes drawing commands
        vk::Queue     _present_queue;        // displays rendering results
        vk::Semaphore _command_complete_sem; // signaled when the rendering process completes
        vk::Semaphore _present_complete_sem; // signaled when the presentation process completes

        vk::UniqueRenderPass _renderpass;

        vk::CommandBuffer _render_cmdbuffer;

        std::vector<frame_attachments> _frame_attachments;
    };

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_FORWARD_RENDERER_HPP