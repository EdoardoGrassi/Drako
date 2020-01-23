#include "drako/graphics/vulkan_forward_renderer.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include "drako/graphics/vulkan_gpu_shader.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_queue.hpp"
#include "drako/graphics/vulkan_swapchain.hpp"

#include "drako/math/mat4x4.hpp"
#include "drako/math/vector3.hpp"

#include <chrono>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    vulkan_forward_renderer::vulkan_forward_renderer(
        vk::PhysicalDevice pdevice,
        vk::Device         device,
        vk::SurfaceKHR     surface,
        uint32_t           width,
        uint32_t           height) noexcept
        : _ldevice(device)
        , _surface(surface)
    {
        DRAKO_ASSERT(device != vk::Device{ nullptr });
        vk::Result result;

        // TODO: from Vulkan 1.0.129 semaphore should have VK_SEMAPHORE_TYPE_BINARY_KHR flag added
        const vk::SemaphoreCreateInfo semaphore_create_info{ vk::SemaphoreCreateFlags{} };
        std::tie(result, _command_complete_sem) = _ldevice.createSemaphore(semaphore_create_info);
        if (result != vk::Result::eSuccess)
        {
            DRAKO_LOG_FAILURE("[Vulkan] Semaphore creation failed - " + to_string(result));
            std::exit(EXIT_FAILURE);
        }

        // TODO: from Vulkan 1.0.129 semaphore should have VK_SEMAPHORE_TYPE_BINARY_KHR flag added
        std::tie(result, _present_complete_sem) = _ldevice.createSemaphore(semaphore_create_info);
        if (result != vk::Result::eSuccess)
        {
            DRAKO_LOG_FAILURE("[Vulkan] Semaphore creation failed - " + to_string(result));
            std::exit(EXIT_FAILURE);
        }

        setup_renderpass();
        setup_swapchain(pdevice);
        const auto [swapchain_images_result, swapchain_images] = _ldevice.getSwapchainImagesKHR(_swapchain.get());
        if (DRAKO_UNLIKELY(swapchain_images_result != vk::Result::eSuccess))
        {
            DRAKO_LOG_FAILURE("[Vulkan] Failed to retrieve swapchain images - " + to_string(swapchain_images_result));
            std::exit(EXIT_FAILURE);
        }

        _frame_attachments = std::vector<frame_attachments>(std::size(swapchain_images));
        for (auto i = 0; i < std::size(swapchain_images); ++i)
        {
            const vk::ImageViewCreateInfo color_buffer_info{
                vk::ImageViewCreateFlags{},
                swapchain_images[i],
                vk::ImageViewType::e2D,
                vk::Format::eUndefined,
                vk::ComponentMapping{},
                vk::ImageSubresourceRange{
                    vk::ImageAspectFlagBits::eColor,
                    0, // base mip level
                    1, // level count
                    0, // base array layer
                    1  // layer count
                }
            };
            if (std::tie(result, _frame_attachments[i].color_buffer_view) = _ldevice.createImageView(color_buffer_info);
                DRAKO_UNLIKELY(result != vk::Result::eSuccess))
            {
                DRAKO_LOG_FAILURE("[Vulkan] " + to_string(result));
                std::exit(EXIT_FAILURE);
            }

            const vk::ImageCreateInfo depth_buffer_info{
                vk::ImageCreateFlags{},
                vk::ImageType::e2D,
                vk::Format::eD32Sfloat,
                vk::Extent3D{},
                1, // mip levels
                1, // array layers
                vk::SampleCountFlagBits::e1,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::SharingMode::eExclusive,
                0, nullptr,
                vk::ImageLayout::eDepthStencilAttachmentOptimal
            };
            if (std::tie(result, _frame_attachments[i].depth_buffer) = _ldevice.createImage(depth_buffer_info);
                DRAKO_UNLIKELY(result != vk::Result::eSuccess))
            {
                DRAKO_LOG_FAILURE("[Vulkan] " + to_string(result));
                std::exit(EXIT_FAILURE);
            }

            const vk::ImageViewCreateInfo depth_buffer_view_info{
                vk::ImageViewCreateFlags{},
                _frame_attachments[i].depth_buffer,
                vk::ImageViewType::e2D,
                vk::Format::eD32Sfloat,
                vk::ComponentMapping{},
                vk::ImageSubresourceRange{
                    vk::ImageAspectFlagBits::eDepth,
                    0, // base mip level
                    1, // level count
                    0, // base array layer
                    1  // layer count
                }
            };
            if (std::tie(result, _frame_attachments[i].depth_buffer_view) = _ldevice.createImageView(depth_buffer_view_info);
                DRAKO_UNLIKELY(result != vk::Result::eSuccess))
            {
                DRAKO_LOG_FAILURE("[Vulkan] " + to_string(result));
                std::exit(EXIT_FAILURE);
            }

            const vk::ImageView attachments[] = {
                _frame_attachments[i].color_buffer_view,
                _frame_attachments[i].depth_buffer_view
            };

            DRAKO_ASSERT(_renderpass.get(), "Renderpass is invalid");
            const vk::FramebufferCreateInfo framebuffer_info{
                vk::FramebufferCreateFlags{},
                _renderpass.get(),
                static_cast<uint32_t>(std::size(attachments)), attachments,
                width,
                height,
                1
            };
            if (std::tie(result, _frame_attachments[i].framebuffer) = _ldevice.createFramebuffer(framebuffer_info);
                DRAKO_UNLIKELY(result != vk::Result::eSuccess))
            {
                DRAKO_LOG_FAILURE("[Vulkan] Framebuffer creation failed - " + to_string(result));
                std::exit(EXIT_FAILURE);
            }
        }
    }



    vulkan_forward_renderer::~vulkan_forward_renderer() noexcept
    {
        for (auto& frame : _frame_attachments)
        {
            _ldevice.destroy(frame.framebuffer);
            _ldevice.destroy(frame.color_buffer_view);
            _ldevice.destroy(frame.depth_buffer);
            _ldevice.destroy(frame.depth_buffer_view);
        }
    }


    void vulkan_forward_renderer::setup_renderpass() noexcept
    {
        const vk::AttachmentReference color_buffer_ref{ 0, vk::ImageLayout::eColorAttachmentOptimal };
        const vk::AttachmentReference depth_buffer_ref{ 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };

        const vk::AttachmentDescription color_buffer_desc{
            {},
            vk::Format::eB8G8R8Uint,
            vk::SampleCountFlagBits::e4,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR
        };

        const vk::AttachmentDescription depth_buffer_desc{
            {},
            vk::Format::eD16Unorm,
            vk::SampleCountFlagBits::e4,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
            vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eUndefined
        };

        const vk::AttachmentDescription attachments[] = { color_buffer_desc, depth_buffer_desc };

        const vk::SubpassDescription vertex_pass{
            {},
            vk::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, &color_buffer_ref, nullptr, &depth_buffer_ref,
            0, nullptr
        };

        const vk::SubpassDescription subpasses[] = { vertex_pass };

        const vk::RenderPassCreateInfo renderpass_create_info{
            {},
            static_cast<uint32_t>(std::size(attachments)), attachments,
            static_cast<uint32_t>(std::size(subpasses)), subpasses,
            0, nullptr
        };

        if (auto [err, renderpass] = _ldevice.createRenderPassUnique(renderpass_create_info);
            DRAKO_LIKELY(err == vk::Result::eSuccess))
        {
            _renderpass = std::move(renderpass);
        }
        else
        {
            DRAKO_LOG_ERROR("[Vulkan] Renderpass creation failed with error - " + to_string(err));
            std::exit(EXIT_FAILURE);
        }
    }



    void vulkan_forward_renderer::setup_swapchain(vk::PhysicalDevice pdevice) noexcept
    {
        // query and select the presentation surface configuration
        const auto [capabs_result, capabs] = pdevice.getSurfaceCapabilitiesKHR(_surface);
        if (capabs_result != vk::Result::eSuccess)
        {
            DRAKO_LOG_FAILURE("[Vulkan] Failed to query surface capabilities -" + to_string(capabs_result));
            std::exit(EXIT_FAILURE);
        }

        const auto SWAPCHAIN_UNLIMITED_COUNT = 0; // vulkan magic value
        const auto swapchain_image_count     = (capabs.maxImageCount == SWAPCHAIN_UNLIMITED_COUNT)
                                               ? capabs.minImageCount + 1
                                               : capabs.minImageCount + 1;

        const auto SWAPCHAIN_DRIVEN_SIZE = vk::Extent2D(UINT32_MAX, UINT32_MAX); // vulkan magic value
        const auto swapchain_extent      = (capabs.currentExtent == SWAPCHAIN_DRIVEN_SIZE)
                                          ? capabs.maxImageExtent // use max extent available
                                          : capabs.currentExtent;


        // query and select the presentation format for the swapchain
        const auto [formats_result, formats] = pdevice.getSurfaceFormatsKHR(_surface);
        if (formats_result != vk::Result::eSuccess)
        {
            DRAKO_LOG_FAILURE("[Vulkan] Failed to query surface formats - " + to_string(formats_result));
            std::exit(EXIT_FAILURE);
        }

        const auto selector = [](auto x) {
            return x.format == vk::Format::eB8G8R8A8Unorm && x.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        };
        const auto [swapchain_format, swapchain_colorspace] = (std::any_of(formats.cbegin(), formats.cend(), selector))
                                                                  ? std::tuple(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear)
                                                                  : std::tuple(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear);


        // query and select the presentation mode for the swapchain
        const auto [modes_result, modes] = pdevice.getSurfacePresentModesKHR(_surface);
        if (modes_result != vk::Result::eSuccess)
        {
            DRAKO_LOG_FAILURE("[Vulkan] Failed to query surface presentation modes -" + to_string(modes_result));
            std::exit(EXIT_FAILURE);
        }

        const auto swapchain_present_mode = (std::any_of(modes.cbegin(), modes.cend(),
                                                [](auto x) { return x == vk::PresentModeKHR::eMailbox; }))
                                                ? vk::PresentModeKHR::eMailbox
                                                : vk::PresentModeKHR::eFifo;

        const vk::SwapchainCreateInfoKHR swapchain_create_info{
            vk::SwapchainCreateFlagsKHR{},
            _surface,
            swapchain_image_count,
            swapchain_format,
            swapchain_colorspace,
            swapchain_extent,
            1, // non-stereoscopic view
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            static_cast<uint32_t>(0), nullptr,
            capabs.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            swapchain_present_mode,
            vk::Bool32{ VK_TRUE },
            vk::SwapchainKHR{ nullptr }
        };

        if (auto [err, swapchain] = _ldevice.createSwapchainKHRUnique(swapchain_create_info);
            err == vk::Result::eSuccess)
        {
            _swapchain = std::move(swapchain);
        }
        else
        {
            DRAKO_LOG_FAILURE("[Vulkan] Swapchain creation failed - " + to_string(err));
            std::exit(EXIT_FAILURE);
        }
    }


    void vulkan_forward_renderer::render(
        const vulkan_material_pipeline&              pipeline,
        const std::vector<mat4x4>&                   mvps,
        const std::vector<vulkan_mesh_view>&         meshes,
        const std::vector<vulkan_material_instance>& materials) noexcept
    {
        DRAKO_ASSERT(std::size(mvps) == std::size(meshes));

        // const uint64_t MAX_TIMEOUT_NS = 1000 * 1000 * 100; // max timeout in nanoseconds, we use 100ms
        const std::chrono::nanoseconds MAX_TIMEOUT             = std::chrono::microseconds{ 100 };
        const auto [acquire_image_result, present_image_index] = _ldevice.acquireNextImageKHR(
            _swapchain.get(),
            static_cast<uint64_t>(MAX_TIMEOUT.count()),
            _command_complete_sem,
            vk::Fence{ nullptr });
        switch (acquire_image_result)
        {
            case vk::Result::eSuccess:
                break;

            case vk::Result::eTimeout:
                DRAKO_LOG_WARNING("[Vulkan] Swapchain timeout - " + to_string(acquire_image_result));
                return;

            case vk::Result::eNotReady:
                return;

            default:
                DRAKO_LOG_FAILURE("[Vulkan] Swapchain failure - " + to_string(acquire_image_result));
                std::exit(EXIT_FAILURE);
        }

        const vk::RenderPassBeginInfo begin_renderpass_info{
            _renderpass,
            _frame_attachments[present_image_index].framebuffer,
            _render_area,
            0, nullptr
        };
        _render_cmdbuffer.beginRenderPass(begin_renderpass_info, vk::SubpassContents::eInline);


        _render_cmdbuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline_handle());

        for (auto i = 0; i < std::size(mvps); ++i)
        {
            _render_cmdbuffer.pushConstants(pipeline.pipeline_layout_handle(),
                vk::ShaderStageFlagBits::eVertex,
                0,
                static_cast<uint32_t>(sizeof(mat4x4)),
                &mvps[i]);

            const vk::Buffer     buffers[] = { meshes[i].vertex_buffer_handle() };
            const vk::DeviceSize offsets[] = { 0 };
            _render_cmdbuffer.bindVertexBuffers(0, static_cast<uint32_t>(std::size(buffers)), buffers, offsets);

            _render_cmdbuffer.bindIndexBuffer(meshes[i].index_buffer_handle(), vk::DeviceSize{ 0 }, vk::IndexType::eUint16);

            _render_cmdbuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                pipeline.pipeline_layout_handle(),
                0, 1, &pipeline.descriptor_sets()[i], 0, nullptr);

            _render_cmdbuffer.drawIndexed(static_cast<uint32_t>(meshes[i].index_buffer_size()),
                1,  // instances count
                0,  // first index
                0,  // vertex offset
                0); // first instance
        }

        _render_cmdbuffer.endRenderPass();

        const vk::PipelineStageFlags pipeline_sync_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        const vk::Semaphore acquire_sems[] = { _present_complete_sem };
        const vk::Semaphore release_sems[] = { _command_complete_sem };
        DRAKO_ASSERT(std::size(acquire_sems) == std::size(pipeline_sync_stages));

        const vk::SubmitInfo submit_info{
            static_cast<uint32_t>(std::size(acquire_sems)), acquire_sems, pipeline_sync_stages,
            1, &_render_cmdbuffer,
            static_cast<uint32_t>(std::size(acquire_sems)), release_sems
        };

        if (const auto result = _command_queue.submit(1, &submit_info, vk::Fence{ nullptr });
            DRAKO_UNLIKELY(result != vk::Result::eSuccess))
        {
            DRAKO_LOG_ERROR("[Vulkan] Queue submit failed" + to_string(result));
            std::exit(EXIT_FAILURE);
        }


        const uint32_t           swapchain_image_indexes[] = { present_image_index };
        const vk::PresentInfoKHR present_info{
            static_cast<uint32_t>(std::size(release_sems)), release_sems,
            1, &_swapchain.get(), swapchain_image_indexes,
            nullptr
        };

        switch (const auto result = _present_queue.presentKHR(present_info))
        {
            case vk::Result::eSuccess:
                break;

            case vk::Result::eSuboptimalKHR:
                DRAKO_LOG_WARNING("[Vulkan] Swapchain in suboptimal state - " + to_string(result));
                break;

            default:
                DRAKO_LOG_ERROR("[Vulkan] Failed presentation on swapchain - " + to_string(result));
                std::exit(EXIT_FAILURE);
        }
        _present_queue.waitIdle();
    }
} // namespace drako::gpx