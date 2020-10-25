#pragma once
#ifndef DRAKO_VULKAN_MATERIAL_TYPES_HPP
#define DRAKO_VULKAN_MATERIAL_TYPES_HPP

#include "drako/devel/logging.hpp"
#include "drako/graphics/material_types.hpp"
#include "drako/graphics/vulkan_material_pipeline.hpp"

#include <cstddef>

#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    inline const std::size_t DRAKO_MAX_DESCRIPTOR_SETS = 4;

    struct material_instance
    {
        vk::DescriptorSet material_descriptor_set[DRAKO_MAX_DESCRIPTOR_SETS];
    };

} // namespace drako::gpx::vulkan

#endif // !DRAKO_VULKAN_MATERIAL_TYPES_HPP