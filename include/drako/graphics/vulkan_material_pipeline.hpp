#pragma once
#ifndef DRAKO_VULKAN_MATERIAL_PIPELINE_HPP
#define DRAKO_VULKAN_MATERIAL_PIPELINE_HPP

#include <vector>

#include <vulkan/vulkan.hpp>

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include "drako/math/mat4x4.hpp"
#include "drako/math/vector3.hpp"

#include "drako/graphics/material_types.hpp"

#include "drako/graphics/vulkan_gpu_shader.hpp"
#include "drako/graphics/vulkan_material_types.hpp"
#include "drako/graphics/vulkan_uniform_buffer.hpp"

namespace drako::gpx
{
    /*
    const uint32_t MVP_MATRIX_BINDING_NUMBER = 0;
    const uint32_t ALBEDO_MAP_BINDING_NUMBER = 1;
    const uint32_t NORMAL_MAP_BINDING_NUMBER = 2;

    const vk::DescriptorSetLayoutBinding mpv_matrix_binding{
        MVP_MATRIX_BINDING_NUMBER,
        vk::DescriptorType::eUniformBuffer,
        1, // array size
        vk::ShaderStageFlagBits::eVertex,
        nullptr
    };
    const vk::DescriptorSetLayoutBinding albedo_map_binding{
        ALBEDO_MAP_BINDING_NUMBER,
        vk::DescriptorType::eCombinedImageSampler,
        1, // array size
        vk::ShaderStageFlagBits::eVertex,
        &_shared_immutable_sampler.get()
    };
    const vk::DescriptorSetLayoutBinding normal_map_binding{
        NORMAL_MAP_BINDING_NUMBER,
        vk::DescriptorType::eCombinedImageSampler,
        1, // array size
        vk::ShaderStageFlagBits::eVertex,
        &_shared_immutable_sampler.get()
    };

    const vk::DescriptorSetLayoutBinding BASE_BINDINGS[] = { mpv_matrix_binding, albedo_map_binding, normal_map_binding };
    */

    vk::DescriptorSetLayoutBinding make_vulkan_binding(const material_data_binding& b) noexcept
    {
        return vk::DescriptorSetLayoutBinding{
            b.shader_binding_attribute,
            vk::DescriptorType::eSampler,
            b.shader_resource_count,
            vk::ShaderStageFlagBits::eVertex,
            nullptr
        };
    }

    class vulkan_material_pipeline
    {
    public:
        explicit vulkan_material_pipeline(const vk::Device d, const vk::RenderPass rp, const material_template& mtl);

        [[nodiscard]] vk::Pipeline pipeline_handle() const noexcept
        {
            return _pipeline.get();
        }

        [[nodiscard]] vk::PipelineLayout pipeline_layout_handle() const noexcept
        {
            return _pipeline_layout.get();
        }

        [[nodiscard]] const std::vector<vk::DescriptorSet>& descriptor_sets() const noexcept
        {
            return _descriptor_sets;
        }

        void bind(uint32_t index, vk::ImageView resource, uint32_t binding, uint32_t array_element) noexcept;

    private:
        const vk::Device              _device;
        const vk::RenderPass          _renderpass;
        vk::UniquePipeline            _pipeline;
        vk::UniquePipelineLayout      _pipeline_layout;
        vk::UniqueDescriptorSetLayout _descriptor_layout;
        vk::UniqueDescriptorPool      _descriptor_pool;
        vk::UniqueSampler             _shared_immutable_sampler;

        std::vector<vk::DescriptorSet> _descriptor_sets{ 100 };

        void setup_pipeline(const vulkan_gpu_shader& vert, const vulkan_gpu_shader& frag) noexcept;
    };

    vulkan_material_pipeline::vulkan_material_pipeline(
        const vk::Device         d,
        const vk::RenderPass     rp,
        const material_template& mtl)
        : _device(d)
        , _renderpass(rp)
    {
        DRAKO_ASSERT(d != vk::Device{}, "Invalid handle");
        DRAKO_ASSERT(rp != vk::RenderPass{}, "Invalid handle");

        const vk::SamplerCreateInfo shared_sampler_info{
            vk::SamplerCreateFlags{},
            vk::Filter::eNearest,
            vk::Filter::eNearest,
            vk::SamplerMipmapMode::eNearest,

            vk::SamplerAddressMode::eClampToEdge,
            vk::SamplerAddressMode::eClampToEdge,
            vk::SamplerAddressMode::eClampToEdge,
            0.f,
            vk::Bool32{ VK_FALSE }, // disable anisotropic filtering
            {},
            vk::Bool32{ VK_FALSE }, // disable prefetching comparison
            {},
            {},
            {},
            vk::BorderColor::eIntOpaqueBlack,
            vk::Bool32{ VK_FALSE } // disable unnormalized coordinates
        };
        if (auto [err, sampler] = _device.createSamplerUnique(shared_sampler_info);
            err == vk::Result::eSuccess)
        {
            _shared_immutable_sampler = std::move(sampler);
        }
        else
        {
            std::exit(EXIT_FAILURE);
        }

        // TODO: vvv review this vvv
        std::vector<vk::DescriptorSetLayoutBinding> bindings(std::size(mtl.bindings));
        for (const auto& binding : mtl.bindings)
        {
            bindings.emplace_back(make_vulkan_binding(binding));
        }
        // ^^^ end review ^^^

        const vk::DescriptorSetLayoutCreateInfo layout_info{
            vk::DescriptorSetLayoutCreateFlags{},
            static_cast<uint32_t>(std::size(bindings)), bindings.data()
        };

        if (auto [err, layout] = _device.createDescriptorSetLayoutUnique(layout_info);
            err == vk::Result::eSuccess)
        {
            _descriptor_layout = std::move(layout);
        }
        else
        {
            std::exit(EXIT_FAILURE);
        }

        const vk::DescriptorPoolSize pool_sizes[] = {
            { vk::DescriptorType::eCombinedImageSampler, 100 }
        };
        const vk::DescriptorPoolCreateInfo pool_info{
            vk::DescriptorPoolCreateFlags{}, // vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            100,
            static_cast<uint32_t>(std::size(pool_sizes)), pool_sizes
        };
        if (auto [err, pool] = _device.createDescriptorPoolUnique(pool_info);
            DRAKO_LIKELY(err == vk::Result::eSuccess))
        {
            _descriptor_pool = std::move(pool);
        }
        else
        {
            std::exit(EXIT_FAILURE);
        }

        const vk::DescriptorSetAllocateInfo preallocation_info{
            _descriptor_pool.get(),
            100,
        };
        if (const auto err = _device.allocateDescriptorSets(&preallocation_info, _descriptor_sets.data());
            err != vk::Result::eSuccess)
        {
            std::exit(EXIT_FAILURE);
        }

        const vk::WriteDescriptorSet inits[] = {};
    }

    void vulkan_material_pipeline::setup_pipeline(const vulkan_gpu_shader& vert, const vulkan_gpu_shader& frag) noexcept
    {
        const vk::PipelineShaderStageCreateInfo shader_stages[] = {
            // vertex shader stage
            {
                vk::PipelineShaderStageCreateFlags{},
                vk::ShaderStageFlagBits::eVertex,
                vert.shader_module(),
                "main",
                static_cast<vk::SpecializationInfo*>(nullptr) },
            // fragment shader stage
            {
                vk::PipelineShaderStageCreateFlags{},
                vk::ShaderStageFlagBits::eFragment,
                frag.shader_module(),
                "main",
                static_cast<vk::SpecializationInfo*>(nullptr) }
        };

        const uint32_t SHADER_BINDING_NUMBER = 0;
        const uint32_t BUFFER_BINDING_NUMBER = 0;

        const vk::VertexInputBindingDescription vertex_bindings[] = {
            // vertex position [x,y,z]
            {
                BUFFER_BINDING_NUMBER,       // binding
                sizeof(::drako::vec3),       // stride
                vk::VertexInputRate::eVertex // AoSformat
            }
        };

        const vk::VertexInputAttributeDescription vertex_attributes[] = {
            // vertex position [x,y,z]
            {
                SHADER_BINDING_NUMBER, // shader binding location number
                BUFFER_BINDING_NUMBER, // buffer binding location number
                vk::Format::eR32G32B32Sfloat,
                sizeof(::drako::vec3) // stride
            }
        };

        const vk::PipelineVertexInputStateCreateInfo vertex_input_state{
            vk::PipelineVertexInputStateCreateFlags{},
            static_cast<uint32_t>(std::size(vertex_bindings)), vertex_bindings,
            static_cast<uint32_t>(std::size(vertex_attributes)), vertex_attributes
        };

        const vk::PipelineInputAssemblyStateCreateInfo input_assembly_state{
            vk::PipelineInputAssemblyStateCreateFlags{},
            vk::PrimitiveTopology::eTriangleList,
            vk::Bool32{ VK_FALSE }
        };

        const vk::Viewport viewports[] = {
            {
                0.f,    // x offset
                0.f,    // y offset
                1000.f, // x extent
                1000.f, // y extent
                0.f,    // z min depth
                1.f     // z max depth
            }
        };

        const vk::Rect2D scissors[] = {
            { vk::Offset2D{ 0, 0 }, vk::Extent2D{ 1000, 1000 } }
        };

        DRAKO_ASSERT(std::size(viewports) == std::size(scissors));
        const vk::PipelineViewportStateCreateInfo viewport_state{
            vk::PipelineViewportStateCreateFlags{},
            static_cast<uint32_t>(std::size(viewports)), viewports,
            static_cast<uint32_t>(std::size(scissors)), scissors
        };

        const vk::PipelineRasterizationStateCreateInfo rasterization_state{
            {},
            vk::Bool32{ VK_FALSE }, // depth clamping
            vk::Bool32{ VK_FALSE }, // rasterizer discard
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack,
            vk::FrontFace::eCounterClockwise,
            vk::Bool32{ VK_FALSE }, // depth bias
            {},                     // depth bias constant factor
            {},                     // depth bias clamp
            {},                     // depth bias slope factor
            {}                      // line width
        };

        const vk::PipelineMultisampleStateCreateInfo multisample_state{
            {},
            vk::SampleCountFlagBits::e8,
            vk::Bool32{ VK_FALSE }, // sample shading
            {},
            nullptr,
            vk::Bool32{ VK_FALSE }, // alpha to coverage
            vk::Bool32{ VK_FALSE }  // alpha to one
        };

        const vk::PipelineDepthStencilStateCreateInfo depth_stencil_state{
            {},
            vk::Bool32{ VK_TRUE }, // depth test
            vk::Bool32{ VK_TRUE }, // depth writing
            vk::CompareOp::eLess,
            vk::Bool32{ VK_FALSE }, // depth bounds test
            vk::Bool32{ VK_FALSE }, // stencil test
            {},
            {},
            {},
            {}
        };

        /*
        const vk::PipelineColorBlendStateCreateInfo color_blend_state
        {
            {
            },
            vk::Bool
        }*/

        const vk::PipelineDynamicStateCreateInfo dynamic_state{
            {},
            0, nullptr
        };

        // const vk::DescriptorSetLayout descriptors[] = {};

        const vk::PushConstantRange push_constants[] = {
            // model-view-proj matrix
            {
                vk::ShaderStageFlagBits::eVertex, // vertex shader
                0,                                // binding number
                sizeof(::drako::mat4x4)           // stride
            }
        };
        const vk::PipelineLayoutCreateInfo pipeline_layout_info{
            vk::PipelineLayoutCreateFlags{},
            static_cast<uint32_t>(1), &_descriptor_layout.get(),
            static_cast<uint32_t>(std::size(push_constants)), push_constants
        };

        if (auto [err, layout] = _device.createPipelineLayoutUnique(pipeline_layout_info);
            err == vk::Result::eSuccess)
        {
            _pipeline_layout = std::move(layout);
        }
        else
        {
            DRAKO_LOG_FAILURE("[Vulkan] Pipeline layout creation failed - " + to_string(err));
            std::exit(EXIT_FAILURE);
        }

        const vk::GraphicsPipelineCreateInfo graphics_pipeline_info{
            vk::PipelineCreateFlags{},
            static_cast<uint32_t>(std::size(shader_stages)), shader_stages,
            &vertex_input_state,
            &input_assembly_state,
            static_cast<vk::PipelineTessellationStateCreateInfo*>(nullptr),
            &viewport_state,
            &rasterization_state,
            &multisample_state,
            &depth_stencil_state,
            nullptr,
            &dynamic_state,
            _pipeline_layout,
            _renderpass,
            0,                       // subpass
            vk::Pipeline{ nullptr }, // base pipeline
            0                        // base pipeline index
        };

        if (auto [err, pipeline] = _device.createGraphicsPipelineUnique(vk::PipelineCache{ nullptr }, graphics_pipeline_info);
            DRAKO_LIKELY(err == vk::Result::eSuccess))
        {
            _pipeline = std::move(pipeline);
        }
        else
        {
            DRAKO_LOG_FAILURE("[Vulkan] Pipeline creation failed - " + to_string(err));
            std::exit(EXIT_FAILURE);
        }
    }

    void vulkan_material_pipeline::bind(uint32_t guid, vk::ImageView resource, uint32_t binding, uint32_t array_element) noexcept
    {
        DRAKO_ASSERT(resource != vk::ImageView{}, "Invalid handle");

        const vk::DescriptorImageInfo desc_image_info[] = {
            { vk::Sampler{},
                resource,
                vk::ImageLayout::eShaderReadOnlyOptimal }
        };

        const vk::WriteDescriptorSet desc_write_info[] = {
            { _descriptor_sets[guid],
                binding,
                array_element,
                static_cast<uint32_t>(std::size(desc_image_info)), // descriptor count
                vk::DescriptorType::eCombinedImageSampler,
                desc_image_info,
                nullptr,
                nullptr }
        };
        _device.updateDescriptorSets(
            static_cast<uint32_t>(std::size(desc_write_info)), desc_write_info,
            0, nullptr);
    }

} // namespace drako::gpx

#endif // !DRAKO_VULKAN_MATERIAL_PIPELINE_HPP