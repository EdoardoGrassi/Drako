#pragma once
#ifndef DRAKO_VULKAN_RUNTIME_CONTEXT_HPP
#define DRAKO_VULKAN_RUNTIME_CONTEXT_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/graphics/vulkan_queue.hpp"
#include "drako/system/desktop_window.hpp"

#include <vulkan/vulkan.hpp>

#if defined(DRAKO_PLT_WIN32)
#include <vulkan/vulkan_win32.h> // enables VK_KHR_win32_surface extension
#endif

#include <iostream>
#include <vector>

namespace drako::vulkan
{
    VKAPI_ATTR VkBool32 VKAPI_CALL _debug_message_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*                                       pUserData)
    {
        std::cerr << "[Vulkan] Validation layer: " << pCallbackData->pMessage << '\n';
        return VK_FALSE;
    }


    [[nodiscard]] vk::UniqueInstance make_instance()
    {
        DRAKO_LOG_INFO("[Vulkan] Instance creation started...");

        const vk::ApplicationInfo app_info{
            "Gui Application with Drako",
            VK_MAKE_VERSION(1, 0, 0),
            "DrakoEngine",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_1
        };

        for (const auto& p : vk::enumerateInstanceLayerProperties())
            std::cout << "Name:\t" << p.layerName << '\n'
                      << "Impl:\t" << p.implementationVersion << '\n'
                      << "Desc:\t" << p.description << '\n';

        for (const auto& e : vk::enumerateInstanceExtensionProperties())
            std::cout << "Name:\t" << e.extensionName << '\n'
                      << "Spec:\t" << e.specVersion << '\n';

        // Enabled layers names
        const char* enabled_layers[] = { "VK_LAYER_KHRONOS_validation" };
        // Enabled extensions names
        const char* enabled_extensions[] = {   // TODO: conditionally remove VK_EXT_debug_utils
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME, // debugging of instance creation
#if defined(DRAKO_PLT_WIN32)
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME // rendering on Windows native surface
#elif defined(DRAKO_PLT_LINUX)

#elif defined(DRAKO_PLT_MACOS)

#endif
        };

        vk::InstanceCreateInfo instance_info{
            vk::InstanceCreateFlags{},
            &app_info,
            static_cast<uint32_t>(std::size(enabled_layers)), enabled_layers,
            static_cast<uint32_t>(std::size(enabled_extensions)), enabled_extensions
        };

        // TODO: change debug settings based on project build type
        vk::DebugUtilsMessengerCreateInfoEXT debug_callback_info{
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            _debug_message_callback,
            nullptr
        };
        vk::StructureChain<decltype(instance_info), decltype(debug_callback_info)> instance_info_chain{
            instance_info,
            debug_callback_info
        };
        return vk::createInstanceUnique(instance_info_chain.get<decltype(instance_info)>());
    }


    [[nodiscard]] vk::UniqueSurfaceKHR make_surface(vk::Instance instance, const sys::desktop_window& w)
    {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        const vk::Win32SurfaceCreateInfoKHR surface_create_info{
            vk::Win32SurfaceCreateFlagsKHR{},
            w.instance_win32(),
            w.handle_win32()
        };
        return instance.createWin32SurfaceKHRUnique(surface_create_info);

#elif defined(VK_USE_PLATFORM_XCB_KHR)
        VkXcbSurfaceCreateInfoKHR surface_create_info = {
            VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR, // VkStructureType                  sType
            nullptr,                                       // const void                      *pNext
            0,                                             // VkXcbSurfaceCreateFlagsKHR       flags
            Window.Connection,                             // xcb_connection_t*                connection
            Window.Handle                                  // xcb_window_t                     window
        };

        if (vkCreateXcbSurfaceKHR(Vulkan.Instance, &surface_create_info, nullptr, &Vulkan.PresentationSurface) == VK_SUCCESS)
        {
            return true;
        }

#elif defined(VK_USE_PLATFORM_XLIB_KHR)
        VkXlibSurfaceCreateInfoKHR surface_create_info = {
            VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR, // VkStructureType                sType
            nullptr,                                        // const void                    *pNext
            0,                                              // VkXlibSurfaceCreateFlagsKHR    flags
            Window.DisplayPtr,                              // Display                       *dpy
            Window.Handle                                   // Window                         window
        };
        if (vkCreateXlibSurfaceKHR(Vulkan.Instance, &surface_create_info, nullptr, &Vulkan.PresentationSurface) == VK_SUCCESS)
        {
            return true;
        }
#else
#error Platform not supported!
#endif
    }



    [[nodiscard]] vk::UniqueSwapchainKHR
    make_swapchain(vk::PhysicalDevice p, vk::Device l, vk::SurfaceKHR s) noexcept
    {
        // query and select the presentation surface configuration
        const auto capabs = p.getSurfaceCapabilitiesKHR(s);

        const auto SWAPCHAIN_UNLIMITED_COUNT = 0; // vulkan magic value
        const auto sc_image_count            = (capabs.maxImageCount == SWAPCHAIN_UNLIMITED_COUNT)
                                        ? capabs.minImageCount + 1
                                        : capabs.minImageCount + 1;

        const auto SWAPCHAIN_DRIVEN_SIZE = vk::Extent2D(UINT32_MAX, UINT32_MAX); // vulkan magic value
        const auto sc_extent             = (capabs.currentExtent == SWAPCHAIN_DRIVEN_SIZE)
                                   ? capabs.maxImageExtent // use max extent available
                                   : capabs.currentExtent;


        // query and select the presentation format for the swapchain
        const auto formats = p.getSurfaceFormatsKHR(s);

        const auto selector = [](auto x) {
            return x.format == vk::Format::eB8G8R8A8Unorm && x.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        };
        const auto [sc_format, sc_colorspace] = (std::any_of(formats.cbegin(), formats.cend(), selector))
                                                    ? std::tuple(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear)
                                                    : std::tuple(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear);


        // query and select the presentation mode for the swapchain
        const auto modes = p.getSurfacePresentModesKHR(s);

        const auto sc_present_mode = (std::any_of(modes.cbegin(), modes.cend(),
                                         [](auto x) { return x == vk::PresentModeKHR::eMailbox; }))
                                         ? vk::PresentModeKHR::eMailbox
                                         : vk::PresentModeKHR::eFifo;

        const uint32_t queue_family_indexes[] = { 0 };

        const vk::SwapchainCreateInfoKHR swapchain_create_info{
            {},
            s,
            sc_image_count,
            sc_format,
            sc_colorspace,
            sc_extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            static_cast<uint32_t>(std::size(queue_family_indexes)), queue_family_indexes,
            capabs.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            sc_present_mode,
            vk::Bool32{ VK_TRUE },
            vk::SwapchainKHR{ nullptr }
        };
        return l.createSwapchainKHRUnique(swapchain_create_info);
    }


    /// @brief Context of a Vulkan application.
    struct context
    {
        explicit context(const sys::desktop_window& w) noexcept;

        context(const context&) = delete;
        context& operator=(const context&) = delete;

        context(context&&) = delete;
        context& operator=(context&&) = delete;

        vk::UniqueInstance               instance;
        vk::PhysicalDevice               physical_device;
        vk::UniqueDevice                 logical_device;
        vk::UniqueSurfaceKHR             surface;
        vk::DispatchLoaderDynamic        dld;
        vk::UniqueDebugUtilsMessengerEXT debug;
    };

    context::context(const sys::desktop_window& w) noexcept
        : instance(make_instance())
    {
        physical_device = make_physical_device(instance.get());
        surface         = make_surface(instance.get(), w);
        logical_device  = make_logical_device(physical_device);

        dld = { instance.get(), logical_device.get() };

        const vk::DebugUtilsMessengerCreateInfoEXT debug_messenger_info{
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            _debug_message_callback,
            nullptr
        };
        debug = instance.get().createDebugUtilsMessengerEXTUnique(debug_messenger_info);
    }

    void debug_print_all_queue_families(const context& ctx)
    {
        const auto properties = ctx.physical_device.getQueueFamilyProperties();
        std::cout << "[Vulkan] Available queue families:\n";
        for (const auto& family : properties)
        {
            std::cout << "\tQueue family:\n"
                      << "\tcount: " << family.queueCount << '\n'
                      << "\tflags: " << to_string(family.queueFlags) << '\n'
                      << "\tmin transfer granularity:\n"
                      << "\t\t- width:  " << family.minImageTransferGranularity.width << '\n'
                      << "\t\t- height: " << family.minImageTransferGranularity.height << '\n'
                      << "\t\t- depth:  " << family.minImageTransferGranularity.depth << '\n';
        }
    }

} // namespace drako::vulkan

#endif // !DRAKO_VULKAN_RUNTIME_CONTEXT_HPP