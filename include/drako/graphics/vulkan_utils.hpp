#pragma once
#ifndef DRAKO_VULKAN_UTILS_HPP
#define DRAKO_VULKAN_UTILS_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/logging.hpp"

#include <vulkan/vulkan.hpp>

#define DRAKO_VK_CHECK_SUCCESS(x) \
    if (const vk::Result err = (x); DRAKO_UNLIKELY(err != vk::Result::eSuccess)) \
    { \
        DRAKO_LOG_ERROR("[VULKAN] Error " + vk::to_string(err)); \
    }

#endif // !DRAKO_VULKAN_UTILS_HPP