#pragma once
#ifndef DRAKO_VULKAN_SHADER_HPP
#define DRAKO_VULKAN_SHADER_HPP

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"
#include "drako/graphics/shader_types.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    /// @brief A vulkan shader object.
    class gpu_shader
    {
    public:
        explicit gpu_shader(vk::Device device, const shader_source& source);

        gpu_shader(const gpu_shader&) = delete;
        gpu_shader& operator=(const gpu_shader&) = delete;

        gpu_shader(gpu_shader&&) = delete;
        gpu_shader& operator=(gpu_shader&&) = delete;

        [[nodiscard]] const vk::ShaderModule& shader_module() const noexcept
        {
            return _module.get();
        }

        // Returns the identifier of the entry function of vertex stage.
        [[nodiscard]] static constexpr auto vertex_function_name() noexcept
        {
            return "main";
        }

    private:
        vk::UniqueShaderModule _module;
    };

    gpu_shader::gpu_shader(vk::Device device, const shader_source& source)
    {
        DRAKO_ASSERT(device != vk::Device{ nullptr });
        DRAKO_ASSERT(source.size() > 0);
        DRAKO_ASSERT(source.size() % 4 == 0, "Required by Vulkan standard");

        const vk::ShaderModuleCreateInfo shader_module{
            vk::ShaderModuleCreateFlags{},
            std::size(source),
            reinterpret_cast<const uint32_t*>(source.data())
        };

        _module = device.createShaderModuleUnique(shader_module);
    }

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_SHADER_HPP