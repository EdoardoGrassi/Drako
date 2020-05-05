#pragma once
#ifndef DRAKO_VULKAN_TEXTURE_TYPES_HPP
#define DRAKO_VULKAN_TEXTURE_TYPES_HPP

#include <vulkan/vulkan.hpp>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"
#include "drako/graphics/texture_types.hpp"

namespace drako::gpx
{
    // Texture resource hosted on GPU memory that can be accessed by shaders.
    class vulkan_gpu_tex2d
    {
    public:
        explicit vulkan_gpu_tex2d(vk::Device device, const texture_2d_view& tex) noexcept;

    private:
        const vk::Device    _device;
        vk::UniqueImage     _image;
        vk::UniqueImageView _view;
    };


    template <typename VulkanAllocator>
    class vulkan_texture
    {
    };

    class vulkan_texture_view
    {
    };

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_TEXTURE_TYPES_HPP