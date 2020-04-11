#pragma once
#ifndef DRAKO_VULKAN_RUNTIME_CONTEXT_HPP_
#define DRAKO_VULKAN_RUNTIME_CONTEXT_HPP_

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/system/native_window.hpp"

#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>

#if defined(DRAKO_PLT_WIN32)
#include <vulkan/vulkan_win32.h> // enables VK_KHR_win32_surface extension
#endif

namespace drako::gpx
{
    VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*                                       pUserData)
    {
        std::cerr << "[Vulkan] Validation layer: " << pCallbackData->pMessage << '\n';
        return VK_FALSE;
    }


    [[nodiscard]] vk::UniqueInstance make_vulkan_instance() noexcept
    {
        DRAKO_LOG_INFO("[Vulkan] Instance creation started...");

        const vk::ApplicationInfo app_info{
            "Gui Application with Drako",
            VK_MAKE_VERSION(1, 0, 0),
            "DrakoEngine",
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_1
        };

        if (const auto [err, layer_properties] = vk::enumerateInstanceLayerProperties();
            err == vk::Result::eSuccess)
        {
            for (const auto& p : layer_properties)
                std::cout << "Name:\t" << p.layerName << '\n'
                          << "Impl:\t" << p.implementationVersion << '\n'
                          << "Desc:\t" << p.description << '\n';
        }
        else
        {
            DRAKO_LOG_ERROR("[Vulkan] " + to_string(err));
            std::exit(EXIT_FAILURE);
        }

        if (const auto [err, extensions] = vk::enumerateInstanceExtensionProperties();
            err == vk::Result::eSuccess)
        {
            for (const auto& e : extensions)
                std::cout << "Name:\t" << e.extensionName << '\n'
                          << "Spec:\t" << e.specVersion << '\n';
        }
        else
        {
            DRAKO_LOG_ERROR("[Vulkan] " + to_string(err));
            std::exit(EXIT_FAILURE);
        }

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
            vulkan_debug_callback,
            nullptr
        };

        vk::StructureChain<decltype(instance_info), decltype(debug_callback_info)> instance_info_chain{
            instance_info,
            debug_callback_info
        };

        auto [result, instance] = vk::createInstanceUnique(instance_info_chain.get<decltype(instance_info)>());
        if (result != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }

        DRAKO_LOG_INFO("[Vulkan] Instance created");
        return instance;
    }


    [[nodiscard]] vk::ResultValue<vk::UniqueSurfaceKHR>
    make_vulkan_surface(vk::Instance instance, const sys::native_window& w) noexcept
    {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        const vk::Win32SurfaceCreateInfoKHR surface_create_info{
            vk::Win32SurfaceCreateFlagsKHR{},
            w.instance_handle_win32(),
            w.window_handle_win32()
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
#endif
    }



    [[nodiscard]] vk::ResultValue<vk::UniqueSwapchainKHR>
    make_vulkan_swapchain(vk::PhysicalDevice p, vk::Device l, vk::SurfaceKHR s) noexcept
    {
        // query and select the presentation surface configuration
        const auto [capabs_result, capabs] = p.getSurfaceCapabilitiesKHR(s);
        if (capabs_result != vk::Result::eSuccess)
        {
            DRAKO_LOG_FAILURE("[Vulkan] Failed to query surface capabilities: " + to_string(capabs_result));
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
        const auto [formats_result, formats] = p.getSurfaceFormatsKHR(s);
        if (formats_result != vk::Result::eSuccess)
        {
            DRAKO_LOG_FAILURE("[Vulkan] Failed to query surface formats: " + to_string(formats_result));
            std::exit(EXIT_FAILURE);
        }

        const auto selector = [](auto x) {
            return x.format == vk::Format::eB8G8R8A8Unorm && x.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        };
        const auto [swapchain_format, swapchain_colorspace] = (std::any_of(formats.cbegin(), formats.cend(), selector))
                                                                  ? std::tuple(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear)
                                                                  : std::tuple(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear);


        // query and select the presentation mode for the swapchain
        const auto [modes_result, modes] = p.getSurfacePresentModesKHR(s);
        if (modes_result != vk::Result::eSuccess)
        {
            DRAKO_LOG_FAILURE("[Vulkan] Failed to query surface presentation modes: " + to_string(modes_result));
            std::exit(EXIT_FAILURE);
        }

        const auto swapchain_present_mode = (std::any_of(modes.cbegin(), modes.cend(),
                                                [](auto x) { return x == vk::PresentModeKHR::eMailbox; }))
                                                ? vk::PresentModeKHR::eMailbox
                                                : vk::PresentModeKHR::eFifo;

        const uint32_t queue_family_indexes[] = { 0 };

        const vk::SwapchainCreateInfoKHR swapchain_create_info{
            {},
            s,
            swapchain_image_count,
            swapchain_format,
            swapchain_colorspace,
            swapchain_extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            static_cast<uint32_t>(std::size(queue_family_indexes)), queue_family_indexes,
            capabs.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            swapchain_present_mode,
            vk::Bool32{ VK_TRUE },
            vk::SwapchainKHR{ nullptr }
        };
        return l.createSwapchainKHRUnique(swapchain_create_info);
    }


    struct vulkan_runtime_context
    {
        explicit vulkan_runtime_context(const sys::native_window& w)
        {
            instance = make_vulkan_instance();

            if (const auto [err, pdevice] = make_vulkan_physical_device(instance.get());
                err == vk::Result::eSuccess)
            {
                physical_device = std::move(pdevice);
            }
            else
            {
                DRAKO_LOG_FAILURE("[Vulkan] " + to_string(err));
                std::exit(EXIT_FAILURE);
            }

            if (auto [err, surface] = make_vulkan_surface(instance.get(), w);
                err == vk::Result::eSuccess)
            {
                surface = std::move(surface);
            }
            else
            {
                DRAKO_LOG_FAILURE("[Vulkan] " + to_string(err));
                std::exit(EXIT_FAILURE);
            }

            if (auto [err, device] = make_vulkan_logical_device(physical_device);
                err == vk::Result::eSuccess)
            {
                logical_device = std::move(device);
            }
            else
            {
                DRAKO_LOG_FAILURE("[Vulkan] " + to_string(err));
                std::exit(EXIT_FAILURE);
            }

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
                vulkan_debug_callback,
                nullptr
            };

            if (auto [err, ris] = instance.get().createDebugUtilsMessengerEXTUnique(debug_messenger_info, nullptr);
                err == vk::Result::eSuccess)
            {
                debug = std::move(ris);
            }
            else
            {
                DRAKO_LOG_FAILURE("[Vulkan] " + to_string(err));
                std::exit(EXIT_FAILURE);
            }
        }

        vk::UniqueInstance               instance;
        vk::PhysicalDevice               physical_device;
        vk::UniqueDevice                 logical_device;
        vk::UniqueSurfaceKHR             surface;
        vk::DispatchLoaderDynamic        dld;
        vk::UniqueDebugUtilsMessengerEXT debug;
    };

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_RUNTIME_CONTEXT_HPP_
