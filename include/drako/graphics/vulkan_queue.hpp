#pragma once
#ifndef DRAKO_VULKAN_QUEUE_HPP
#define DRAKO_VULKAN_QUEUE_HPP

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    using vulkan_queue_index  = std::uint32_t;
    using vulkan_queue_family = std::uint32_t;
    using vulkan_queue_prio   = float;

    struct _basic_queue
    {
        vk::Queue           queue;
        vulkan_queue_index  index;
        vulkan_queue_family family;
        vulkan_queue_prio   prio;
    };

    struct transfer_queue : _basic_queue
    {
    };

    struct graphic_queue : transfer_queue
    {
    };

    struct compute_queue : transfer_queue
    {
    };

    struct generic_queue : graphic_queue, compute_queue
    {
    };

    struct device_queue_report
    {
        std::vector<graphic_queue>  optimal_graphic_queues;
        std::vector<compute_queue>  optimal_compute_queues;
        std::vector<transfer_queue> optimal_transfer_queues;
        std::vector<generic_queue>  generic_queues;
    };


    [[nodiscard]] vk::PhysicalDevice make_physical_device(vk::Instance i) noexcept
    {
        DRAKO_ASSERT(i);

        const auto pdevices = i.enumeratePhysicalDevices();
        for (const auto& pdevice : pdevices)
        {
            const auto properties = pdevice.getProperties();
            DRAKO_LOG_INFO("[Vulkan] device name" + std::string(properties.deviceName));

            const auto features = pdevice.getFeatures();
            // DRAKO_LOG_INFO("[Vulkan] " + to_string(features));
        }
        // select standalone gpu chip
        auto optimal_device_matcher = [](const vk::PhysicalDevice d) {
            return d.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
        };
        if (auto device = std::find(
                std::begin(pdevices), std::end(pdevices), optimal_device_matcher);
            device != std::end(pdevices))
        {
            return *device;
        }
        // select first device available
        return pdevices[0];
    }


    [[nodiscard]] vk::UniqueDevice make_logical_device(vk::PhysicalDevice p) noexcept
    {
        DRAKO_ASSERT(p);

        const auto family_properties = p.getQueueFamilyProperties();
        for (const auto& family : family_properties)
        {
            DRAKO_LOG_INFO("[Vulkan]" + to_string(family.queueFlags));
        }

        const float queue_prios[] = { 1.0f };
        DRAKO_ASSERT(std::all_of(std::begin(queue_prios), std::end(queue_prios),
            [](float x) { return x <= 1.f && x >= 0.f; }));

        const vk::DeviceQueueCreateInfo graphic_queue_info{
            vk::DeviceQueueCreateFlags{},
            0,
            static_cast<uint32_t>(std::size(queue_prios)), queue_prios
        };

        const vk::DeviceQueueCreateInfo compute_queue_info{
            vk::DeviceQueueCreateFlags{},
            0,
            static_cast<uint32_t>(std::size(queue_prios)), queue_prios
        };

        const vk::DeviceQueueCreateInfo transfer_queue_info{
            vk::DeviceQueueCreateFlags{},
            0,
            static_cast<uint32_t>(0), queue_prios
        };

        const vk::DeviceQueueCreateInfo enabled_queues[]     = { graphic_queue_info };
        const char*                     enabled_layers[]     = { "VK_LAYER_KHRONOS_validation" };
        const char*                     enabled_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        // const vk::PhysicalDeviceFeatures pdevice_features[]   = {};

        const vk::DeviceCreateInfo device_create_info{
            vk::DeviceCreateFlags{},
            static_cast<uint32_t>(std::size(enabled_queues)), enabled_queues,
            static_cast<uint32_t>(std::size(enabled_layers)), enabled_layers,
            static_cast<uint32_t>(std::size(enabled_extensions)), enabled_extensions,
            nullptr // &pdevice_features
        };
        return p.createDeviceUnique(device_create_info);
    }

} // namespace drako::gpx::vulkan

#endif // !DRAKO_VULKAN_QUEUE_HPP