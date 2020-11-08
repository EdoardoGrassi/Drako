#include "drako/graphics/vulkan_forward_renderer.hpp"

#include "drako/devel/logging.hpp"
#include "drako/graphics/vulkan_gpu_shader.hpp"
#include "drako/graphics/vulkan_mesh_types.hpp"
#include "drako/graphics/vulkan_queue.hpp"
#include "drako/math/mat4x4.hpp"
#include "drako/math/vector3.hpp"

#include <cassert>
#include <chrono>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    forward_renderer::forward_renderer(
        const context& ctx, uint32_t width, uint32_t height)
        : _pdevice(ctx.physical_device)
        , _ldevice(ctx.logical_device.get())
        , _surface(ctx.surface.get())
    {
        assert(width > 0);
        assert(height > 0);

        // TODO: from Vulkan 1.0.129 semaphore should have VK_SEMAPHORE_TYPE_BINARY_KHR flag added
        const vk::SemaphoreCreateInfo semaphore_create_info{ vk::SemaphoreCreateFlags{} };
        _command_complete_sem = _ldevice.createSemaphore(semaphore_create_info);

        // TODO: from Vulkan 1.0.129 semaphore should have VK_SEMAPHORE_TYPE_BINARY_KHR flag added
        _present_complete_sem = _ldevice.createSemaphore(semaphore_create_info);

        _setup_renderpass();
        _setup_swapchain(_pdevice);
        const auto swapchain_images = _ldevice.getSwapchainImagesKHR(_swapchain.get());

        _attachments.reserve(std::size(swapchain_images));
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
            _attachments[i].color_buffer_view = _ldevice.createImageView(color_buffer_info);

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
            _attachments[i].depth_buffer = _ldevice.createImage(depth_buffer_info);

            const vk::ImageViewCreateInfo depth_buffer_view_info{
                vk::ImageViewCreateFlags{},
                _attachments[i].depth_buffer,
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
            _attachments[i].depth_buffer_view = _ldevice.createImageView(depth_buffer_view_info);

            const vk::ImageView attachments[] = {
                _attachments[i].color_buffer_view,
                _attachments[i].depth_buffer_view
            };

            assert(_renderpass.get());
            const vk::FramebufferCreateInfo framebuffer_info{
                vk::FramebufferCreateFlags{},
                _renderpass.get(),
                static_cast<uint32_t>(std::size(attachments)), attachments,
                width,
                height,
                1
            };
            _attachments[i].framebuffer = _ldevice.createFramebuffer(framebuffer_info);
        }
    }



    forward_renderer::~forward_renderer() noexcept
    {
        for (auto& frame : _attachments)
        {
            _ldevice.destroy(frame.framebuffer);
            _ldevice.destroy(frame.color_buffer_view);
            _ldevice.destroy(frame.depth_buffer);
            _ldevice.destroy(frame.depth_buffer_view);
        }
    }


    void forward_renderer::_setup_renderpass() noexcept
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
        _renderpass = _ldevice.createRenderPassUnique(renderpass_create_info);
    }



    void forward_renderer::_setup_swapchain(vk::PhysicalDevice pdevice) noexcept
    {
        // query and select the presentation surface configuration
        const auto capabs = pdevice.getSurfaceCapabilitiesKHR(_surface);

        const auto SWAPCHAIN_UNLIMITED_COUNT = 0; // vulkan magic value
        const auto swapchain_image_count     = (capabs.maxImageCount == SWAPCHAIN_UNLIMITED_COUNT)
                                               ? capabs.minImageCount + 1
                                               : capabs.minImageCount + 1;

        const auto SWAPCHAIN_DRIVEN_SIZE = vk::Extent2D(UINT32_MAX, UINT32_MAX); // vulkan magic value
        const auto swapchain_extent      = (capabs.currentExtent == SWAPCHAIN_DRIVEN_SIZE)
                                          ? capabs.maxImageExtent // use max extent available
                                          : capabs.currentExtent;


        // query and select the presentation format for the swapchain
        const auto formats = pdevice.getSurfaceFormatsKHR(_surface);

        const auto selector = [](auto x) {
            return x.format == vk::Format::eB8G8R8A8Unorm && x.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        };
        const auto [swapchain_format, swapchain_colorspace] = (std::any_of(formats.cbegin(), formats.cend(), selector))
                                                                  ? std::tuple(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear)
                                                                  : std::tuple(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear);


        // query and select the presentation mode for the swapchain
        const auto modes = pdevice.getSurfacePresentModesKHR(_surface);

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

        _swapchain = _ldevice.createSwapchainKHRUnique(swapchain_create_info);
    }


    void forward_renderer::draw(const draw_batch_mtl_soa& batch) noexcept
    {
        assert(std::size(batch.mvps) == std::size(batch.meshes));
        assert(std::size(batch.mvps) == std::size(batch.materials));

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
                log_and_exit("[Vulkan] Swapchain failure - " + to_string(acquire_image_result));
        }

        const vk::RenderPassBeginInfo begin_renderpass_info{
            _renderpass.get(),
            _attachments[present_image_index].framebuffer,
            _render_area,
            0, nullptr
        };
        _command_buffer.beginRenderPass(begin_renderpass_info, vk::SubpassContents::eInline);


        _command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, batch.pipeline.pipeline_handle());

        // bind shared material data
        _command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
            batch.pipeline.pipeline_layout_handle(),
            0,
            std::size(batch.pipeline.descriptor_sets()), batch.pipeline.descriptor_sets().data(),
            0, nullptr);

        for (auto i = 0; i < std::size(batch.mvps); ++i)
        {
            _command_buffer.pushConstants(batch.pipeline.pipeline_layout_handle(),
                vk::ShaderStageFlagBits::eVertex,
                0,
                static_cast<uint32_t>(sizeof(mat4x4)),
                &batch.mvps[i]);

            const vk::Buffer     buffers[] = { batch.meshes[i].vertex_buffer_handle() };
            const vk::DeviceSize offsets[] = { 0 };
            assert(std::size(buffers) == std::size(offsets));

            _command_buffer.bindVertexBuffers(0, static_cast<uint32_t>(std::size(buffers)), buffers, offsets);
            _command_buffer.bindIndexBuffer(batch.meshes[i].index_buffer_handle(), vk::DeviceSize{ 0 }, vk::IndexType::eUint16);

            // bind instance material data
            _command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                batch.pipeline.pipeline_layout_handle(),
                0, 1, &batch.pipeline.descriptor_sets()[i], 0, nullptr);

            _command_buffer.drawIndexed(
                static_cast<uint32_t>(batch.meshes[i].index_buffer_size()),
                1,  // instances count
                0,  // first index
                0,  // vertex offset
                0); // first instance
        }

        _command_buffer.endRenderPass();

        const vk::PipelineStageFlags pipeline_sync_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        const vk::Semaphore acquire_sems[] = { _present_complete_sem };
        const vk::Semaphore release_sems[] = { _command_complete_sem };
        assert(std::size(acquire_sems) == std::size(pipeline_sync_stages));

        const vk::SubmitInfo submit_info{
            static_cast<uint32_t>(std::size(acquire_sems)), acquire_sems, pipeline_sync_stages,
            1, &_command_buffer,
            static_cast<uint32_t>(std::size(acquire_sems)), release_sems
        };

        if (const auto result = _command_queue.submit(1, &submit_info, vk::Fence{ nullptr });
            result != vk::Result::eSuccess)
            [[unlikely]] log_and_exit("[Vulkan] Queue submit failed" + to_string(result));


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
                log_and_exit("[Vulkan] Failed presentation on swapchain - " + to_string(result));
        }
        _present_queue.waitIdle();
    }

} // namespace drako::gpx::vulkan