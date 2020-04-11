#pragma once
#ifndef DRAKO_VULKAN_SHADER_HPP
#define DRAKO_VULKAN_SHADER_HPP

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"
#include "drako/graphics/shader_types.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::gpx
{
    // A vulkan shader object.
    class vulkan_gpu_shader
    {
    public:
        explicit vulkan_gpu_shader(vk::Device device, const shader_source& source) noexcept;

        vulkan_gpu_shader(const vulkan_gpu_shader&) = delete;
        vulkan_gpu_shader& operator=(const vulkan_gpu_shader&) = delete;

        vulkan_gpu_shader(vulkan_gpu_shader&&) = delete;
        vulkan_gpu_shader& operator=(vulkan_gpu_shader&&) = delete;

        [[nodiscard]] const vk::ShaderModule& shader_module() const noexcept
        {
            return _module.get();
        }

        // Returns the identifier of the entry function of vertex stage.
        [[nodiscard]] static constexpr auto vk_vertex_function_name() noexcept
        {
            return "main";
        }

    private:
        vk::UniqueShaderModule _module;
    };

    vulkan_gpu_shader::vulkan_gpu_shader(vk::Device device, const shader_source& source) noexcept
    {
        DRAKO_ASSERT(device != vk::Device{ nullptr });
        DRAKO_ASSERT(source.size() > 0);
        DRAKO_ASSERT(source.size() % 4 == 0, "Required by Vulkan standard");

        const vk::ShaderModuleCreateInfo shader_module{
            vk::ShaderModuleCreateFlags{},
            std::size(source),
            reinterpret_cast<const uint32_t*>(source.data())
        };

        if (auto [err, module] = device.createShaderModuleUnique(shader_module);
            err == vk::Result::eSuccess)
        {
            _module = std::move(module);
        }
        else
        {
            DRAKO_LOG_FAILURE("[Vulkan] Shader object creation failed - " + to_string(err));
            std::exit(EXIT_FAILURE);
        }
    }

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_SHADER_HPP