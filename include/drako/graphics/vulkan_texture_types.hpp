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
        explicit vulkan_gpu_tex2d(vk::Device device, const texture_2d_view& tex) noexcept
            : _device(device)
        {
            DRAKO_ASSERT(device != vk::Device{ nullptr });

            const vk::ImageCreateInfo staging_image_info{
                vk::ImageCreateFlagBits{},
                vk::ImageType::e2D,
                vk::Format::eA8B8G8R8UintPack32,
                vk::Extent3D{ tex.width, tex.height(), 1 },
                1,
                1,
                vk::SampleCountFlagBits::e1,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferSrc,
                vk::SharingMode::eExclusive,
                0,
                nullptr,
                vk::ImageLayout::eTransferSrcOptimal
            };

            auto [staging_image_result, staging_image] = _device.createImage(staging_image_info);
            if (staging_image_result != vk::Result::eSuccess)
            {
                std::exit(EXIT_FAILURE);
            }

            const vk::ImageCreateInfo sample_image_info{
                vk::ImageCreateFlagBits{},
                vk::ImageType::e2D,
                vk::Format::eA8B8G8R8UnormPack32,
                vk::Extent3D{ tex.width(), tex.height(), 1 },
                tex.mipmap_levels(),
                1,
                vk::SampleCountFlagBits::e8,
                vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eTransferDst,
                vk::SharingMode::eExclusive,
                0,
                nullptr,
                vk::ImageLayout::eShaderReadOnlyOptimal
            };

            if (auto [result, image] = _device.createImageUnique(sample_image_info);
                result == vk::Result::eSuccess)
            {
                _image = std::move(image);
            }
            else
            {
                DRAKO_LOG_ERROR("[Vulkan] " + to_string(result));
                std::exit(EXIT_FAILURE);
            }


            const vk::ImageViewCreateInfo sample_view_info{
                vk::ImageViewCreateFlags{},
                _image,
                vk::ImageViewType::e2D,
                vk::Format::eA8B8G8R8UintPack32,
                vk::ComponentMapping{},
                vk::ImageSubresourceRange{
                    vk::ImageAspectFlagBits::eColor,
                    0, tex.mipmap_levels(),
                    0, 1 }
            };
            auto [sample_view_result, sample_view] = _device.createImageView(sample_view_info);
        }

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