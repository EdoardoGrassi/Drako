#pragma once
#ifndef DRAKO_VULKAN_QUEUE_HPP
#define DRAKO_VULKAN_QUEUE_HPP

#include "drako/devel/logging.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace drako::vulkan
{
    struct _basic_queue
    {
        vk::Queue     queue;
        std::uint32_t index;
        std::uint32_t family;
        float         priority;
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


    [[nodiscard]] inline vk::PhysicalDevice make_physical_device(vk::Instance i)
    {
        assert(i != vk::Instance{ nullptr });

        const auto pdevices = i.enumeratePhysicalDevices();
        for (const auto& pdevice : pdevices)
        {
            const auto properties = pdevice.getProperties();
            DRAKO_LOG_INFO("[Vulkan] device name" + std::string{ properties.deviceName });

            const auto features = pdevice.getFeatures();
            //DRAKO_LOG_INFO("[Vulkan] " + to_string(features));
        }
        if (std::empty(pdevices))
            [[unlikely]] throw std::runtime_error{ "No device with Vulkan support." };

        // select standalone gpu chip
        auto optimal_device_matcher = [](const vk::PhysicalDevice d) {
            return d.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
        };
        if (auto device = std::find_if(
                std::cbegin(pdevices), std::cend(pdevices), optimal_device_matcher);
            device != std::cend(pdevices))
        {
            return *device;
        }
        // select first device available
        return pdevices[0];
    }


    [[nodiscard]] inline vk::UniqueDevice make_logical_device(vk::PhysicalDevice p) noexcept
    {
        assert(p != vk::PhysicalDevice{ nullptr });

        const auto family_properties = p.getQueueFamilyProperties();
        DRAKO_LOG_INFO("[Vulkan] Queue family properties:");
        for (const auto& family : family_properties)
        {
            DRAKO_LOG_INFO("[Vulkan]" + to_string(family.queueFlags));
        }

        const float queue_prios[] = { 1.0f };
        assert(std::all_of(std::cbegin(queue_prios), std::cend(queue_prios),
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

        const vk::DeviceQueueCreateInfo queues[]     = { graphic_queue_info };
        const char*                     layers[]     = { "VK_LAYER_KHRONOS_validation" };
        const char*                     extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        // const vk::PhysicalDeviceFeatures pdevice_features[]   = {};

        const vk::DeviceCreateInfo device_create_info{
            vk::DeviceCreateFlags{},
            static_cast<uint32_t>(std::size(queues)), queues,
            static_cast<uint32_t>(std::size(layers)), layers,
            static_cast<uint32_t>(std::size(extensions)), extensions,
            nullptr // &pdevice_features
        };
        return p.createDeviceUnique(device_create_info);
    }

} // namespace drako::vulkan

#endif // !DRAKO_VULKAN_QUEUE_HPP