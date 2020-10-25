#include "drako/graphics/vulkan_gui_renderer.hpp"

#include "drako/graphics/vulkan_gpu_shader.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"

#include <algorithm>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    vk_gui_renderer::vk_gui_renderer(vk::Instance hinstance, vk::SurfaceKHR surface,
        const uint32_t           queue_family_index_,
        const vulkan_gpu_shader& vert,
        const vulkan_gpu_shader& frag) noexcept
        : _instance(hinstance)
        , _surface(surface)
    {
        vk::Result result;

        auto pdevices = _instance.enumeratePhysicalDevices();
        for (const auto& pdevice : pdevices)
        {
            pdevice.getProperties();
            pdevice.getFeatures();
        }

        _pdevice                     = pdevices[0];
        const auto family_properties = _pdevice.getQueueFamilyProperties();

        const float queue_prios[] = { 1.0f };
        DRAKO_ASSERT(std::all_of(std::begin(queue_prios), std::end(queue_prios),
            [](float x) { return x <= 1.f && x >= 0.f; }));

        const vk::DeviceQueueCreateInfo render_queue_info{
            vk::DeviceQueueCreateFlags{},
            0,
            static_cast<uint32_t>(std::size(queue_prios)), queue_prios
        };

        const vk::DeviceQueueCreateInfo enabled_queues[]     = { render_queue_info };
        const char*                     enabled_layers[]     = { "VK_LAYER_KHRONOS_validation" };
        const char*                     enabled_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        // const vk::PhysicalDeviceFeatures pdevice_features{};

        const vk::DeviceCreateInfo ldevice_info{
            vk::DeviceCreateFlags{},
            static_cast<uint32_t>(std::size(enabled_queues)), enabled_queues,
            static_cast<uint32_t>(std::size(enabled_layers)), enabled_layers,
            static_cast<uint32_t>(std::size(enabled_extensions)), enabled_extensions,
            nullptr // &pdevice_features
        };
        std::tie(result, _ldevice) = _pdevice.createDevice(ldevice_info);
        if (result != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }

        setup_swapchain();

        const vk::CommandPoolCreateInfo pool_create_info({}, queue_family_index_);
        _cmd_pool = _ldevice.createCommandPool(pool_create_info);

        setup_polygon_pipeline(vert, frag);

        // Create synchronization objects

        _drawing_complete_sem = _ldevice.createSemaphore({});
        _present_complete_sem = _ldevice.createSemaphore({});

        setup_descriptors();
        setup_renderpass();
        setup_framebuffer(_renderpass);
    }

    vk_gui_renderer::~vk_gui_renderer() noexcept
    {
        _ldevice.destroyFramebuffer(_framebuffer);
        _ldevice.destroyImageView(_color_buffer_view);
        _ldevice.destroyImageView(_depth_buffer_view);

        _ldevice.destroySemaphore(_drawing_complete_sem);
        _ldevice.destroySemaphore(_present_complete_sem);

        _ldevice.destroyRenderPass(_renderpass);
        _ldevice.destroyPipeline(_pipeline);

        _ldevice.destroyDescriptorPool(_desc_pool);
        _ldevice.destroySwapchainKHR(_swapchain);
    }


    void vk_gui_renderer::render(vk::CommandBuffer* buffer) noexcept
    {
        const uint64_t MAX_TIMEOUT_NS                          = 1000 * 1000 * 100; // max timeout in nanoseconds, we use 100ms
        const auto [acquire_image_result, present_image_index] = _ldevice.acquireNextImageKHR(
            _swapchain,
            static_cast<uint64_t>(MAX_TIMEOUT_NS), // max timeout
            _drawing_complete_sem,
            {} /*VK_NULL_HANDLE*/
        );
        switch (acquire_image_result)
        {
            case vk::Result::eSuccess:
                break;

            case vk::Result::eTimeout:
            case vk::Result::eNotReady:
                return;

            default:
                std::exit(EXIT_FAILURE);
        }

        const vk::PipelineStageFlags pipeline_sync_stages[] = {
            vk::PipelineStageFlagBits::eColorAttachmentOutput
        };

        const vk::Semaphore acquire_sems[] = { _present_complete_sem };
        const vk::Semaphore release_sems[] = { _drawing_complete_sem };

        const vk::SubmitInfo submit_info{
            static_cast<uint32_t>(std::size(acquire_sems)), acquire_sems, pipeline_sync_stages,
            1, buffer, // TODO: set handle to cmd buffer
            static_cast<uint32_t>(std::size(release_sems)), release_sems
        };

        auto result = _render_queue.submit(1, &submit_info, vk::Fence{ nullptr });


        const uint32_t           image_indexes[] = { present_image_index };
        const vk::PresentInfoKHR present_info{
            static_cast<uint32_t>(std::size(release_sems)), release_sems,
            1, &_swapchain,
            image_indexes, nullptr
        };

        switch (const auto result = _present_queue.presentKHR(present_info))
        {
            case vk::Result::eSuccess:
                break;

            default:
            {
                DRAKO_LOG_ERROR("[Vulkan] " + to_string(result));
                std::exit(EXIT_FAILURE);
            }
        }
        _present_queue.waitIdle();
    }

    void vk_gui_renderer::setup_polygon_pipeline(const vulkan_gpu_shader& vert, const vulkan_gpu_shader& frag) noexcept
    {
        const vk::DynamicState                   dynamic_states[] = { vk::DynamicState::eViewport };
        const vk::PipelineDynamicStateCreateInfo dynamic_state_info{
            vk::PipelineDynamicStateCreateFlags{},
            static_cast<uint32_t>(std::size(dynamic_states)),
            dynamic_states
        };

        const vk::PipelineShaderStageCreateInfo vert_stage_info{
            {},
            vk::ShaderStageFlagBits::eVertex,
            vert.shader_module(), // TODO: get vulkan shader module
            vulkan_gpu_shader::vk_vertex_function_name(),
            nullptr
        };

        const vk::PipelineShaderStageCreateInfo frag_stage_info{
            {},
            vk::ShaderStageFlagBits::eFragment,
            frag.shader_module(),
            vulkan_gpu_shader::vk_vertex_function_name(),
            nullptr
        };

        const vk::PipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };

        const auto bindings   = gui_vertex_data::vk_input_bindings();
        const auto attributes = gui_vertex_data::vk_input_attributes();

        const vk::PipelineVertexInputStateCreateInfo vertex_input_state_info{
            vk::PipelineVertexInputStateCreateFlags{},
            static_cast<uint32_t>(std::size(bindings)), bindings,
            static_cast<uint32_t>(std::size(attributes)), attributes
        };

        const vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_info{
            {}, vk::PrimitiveTopology::eTriangleList
        };

        const vk::PipelineRasterizationStateCreateInfo rasterization_state_info{
            {},
            vk::Bool32{ VK_FALSE },
            vk::Bool32{ VK_FALSE },
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eNone,
            vk::FrontFace::eCounterClockwise,
            vk::Bool32{ VK_FALSE }
        };

        const vk::PipelineColorBlendStateCreateInfo blend_state_info({}, vk::Bool32{ VK_FALSE });

        const vk::PipelineViewportStateCreateInfo viewport_state_info(
            {},
            0, nullptr,
            0, nullptr);

        const vk::PipelineDepthStencilStateCreateInfo stencil_state_info(
            {},
            vk::Bool32{ VK_TRUE },
            vk::Bool32{ VK_TRUE },
            vk::CompareOp::eGreater);

        const vk::PipelineMultisampleStateCreateInfo multisample_state_info({},
            vk::SampleCountFlagBits::e4);

        const vk::GraphicsPipelineCreateInfo graphics_pipeline_info(
            {},
            static_cast<uint32_t>(std::size(shader_stages)), shader_stages,
            &vertex_input_state_info,
            &input_assembly_state_info,
            nullptr,
            &viewport_state_info,
            &rasterization_state_info,
            &multisample_state_info,
            &stencil_state_info,
            &blend_state_info,
            &dynamic_state_info,
            {},
            {}, 0,
            {}, 0);

        vk::Result result;
        std::tie(result, _pipeline) = _ldevice.createGraphicsPipeline(nullptr, graphics_pipeline_info);
        if (result != vk::Result::eSuccess)
        {
            DRAKO_LOG_ERROR("[Vulkan] " + to_string(result));
            std::exit(EXIT_FAILURE);
        }
    }

    void vk_gui_renderer::setup_descriptors() noexcept
    {
        // Create descriptor sets that defines the interface between shaders and data
        vk::Result result;

        const vk::DescriptorPoolSize desc_pool_sizes[] = {
            { vk::DescriptorType::eUniformBuffer, 1 },
            { vk::DescriptorType::eSampler, 1 }
        };
        const uint32_t                     MAX_DESCRIPTOR_SETS_COUNT = 10;
        const vk::DescriptorPoolCreateInfo desc_pool_info(
            {},
            MAX_DESCRIPTOR_SETS_COUNT,
            static_cast<uint32_t>(std::size(desc_pool_sizes)), desc_pool_sizes);

        std::tie(result, _desc_pool) = _ldevice.createDescriptorPool(desc_pool_info);
        if (result != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }

        const vk::DescriptorSetAllocateInfo desc_allocation_info(_desc_pool, 0, nullptr);
        std::tie(result, _desc_sets) = _ldevice.allocateDescriptorSets(desc_allocation_info);
        if (result != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }
    }

    void vk_gui_renderer::setup_framebuffer(vk::RenderPass renderpass) noexcept
    {
        vk::Result result;

        const vk::ImageView attachments[] = { _color_buffer_view, _depth_buffer_view };
        std::tie(result, _framebuffer)    = _ldevice.createFramebuffer(
            { {},
                renderpass,
                static_cast<uint32_t>(std::size(attachments)), attachments,
                0 /*WIDTH*/,
                0 /*HEIGHT*/,
                1 });
        if (result != vk::Result::eSuccess)
            std::exit(EXIT_FAILURE);
    }

    void vk_gui_renderer::setup_renderpass() noexcept
    {
        vk::Result result;

        const vk::AttachmentReference color_buffer_ref(0, vk::ImageLayout::eColorAttachmentOptimal);
        const vk::AttachmentReference depth_buffer_ref(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

        const vk::AttachmentDescription color_buffer_desc(
            {},
            vk::Format::eB8G8R8Uint,
            vk::SampleCountFlagBits::e4,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);

        const vk::AttachmentDescription depth_buffer_desc(
            {},
            vk::Format::eD16Unorm,
            vk::SampleCountFlagBits::e4,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eUndefined);

        const vk::AttachmentDescription attachments[] = { color_buffer_desc, depth_buffer_desc };

        const vk::SubpassDescription vertex_pass(
            {},
            vk::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, &color_buffer_ref, nullptr, &depth_buffer_ref,
            0, nullptr);

        const vk::SubpassDescription subpasses[] = { vertex_pass };

        const vk::RenderPassCreateInfo renderpass_create_info(
            {},
            static_cast<uint32_t>(std::size(attachments)), attachments,
            static_cast<uint32_t>(std::size(subpasses)), subpasses,
            0, nullptr);
        std::tie(result, _renderpass) = _ldevice.createRenderPass(renderpass_create_info);
        if (result != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }
    }

    void vk_gui_renderer::setup_swapchain() noexcept
    {
        vk::Result result;
        std::tie(result, _swapchain) = make_vulkan_swapchain(_pdevice, _ldevice, _surface);
        if (result != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }
    }

} // namespace drako::gpx