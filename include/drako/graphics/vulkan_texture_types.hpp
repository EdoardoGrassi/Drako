#pragma once
#ifndef DRAKO_VULKAN_TEXTURE_TYPES_HPP
#define DRAKO_VULKAN_TEXTURE_TYPES_HPP

#include "drako/graphics/texture_types.hpp"

#include <vulkan/vulkan.hpp>

namespace drako::vulkan
{
    // Texture resource hosted on GPU memory that can be accessed by shaders.
    class gpu_tex2d
    {
    public:
        explicit gpu_tex2d(vk::Device device, const texture_2d_view& tex) noexcept;

    private:
        const vk::Device    _device;
        vk::UniqueImage     _image;
        vk::UniqueImageView _view;
    };


    template <typename VulkanAllocator>
    class texture
    {
    };

    /// @brief Non-owning view of a texture resource.
    class texture_view
    {
    public:
    private:
        vk::UniqueImage     _image;
        vk::UniqueImageView _view;
    };

} // namespace drako::vulkan

#endif // !DRAKO_VULKAN_TEXTURE_TYPES_HPP