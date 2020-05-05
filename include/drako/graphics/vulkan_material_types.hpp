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

    struct vulkan::material_instance
    {
        vk::DescriptorSet material_descriptor_set[DRAKO_MAX_DESCRIPTOR_SETS];
    };

    vulkan::material_instance compile_material(
        const material_template&        mt,
        const material_instance&        mi,
        const vulkan_material_pipeline& p) noexcept
    {
        vulkan_material_instance instance;

        for (const auto& binding : mt.bindings)
        {
            switch (const auto type = binding.shader_resource_type; type)
            {
                case material_data_type::inline_data:
                {
                    break;
                }
                case material_data_type::sampler_texture_2d:
                {
                    break;
                }
                default:
                    DRAKO_LOG_ERROR("[Drako] Unrecognized resource type");
            }
        }
        return instance;
    }
} // namespace drako::gpx

#endif // !DRAKO_VULKAN_MATERIAL_TYPES_HPP