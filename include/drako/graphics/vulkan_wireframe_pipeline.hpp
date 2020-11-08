#pragma once
#ifndef DRAKO_VULKAN_WIREFRAME_PIPELINE_HPP
#define DRAKO_VULKAN_WIREFRAME_PIPELINE_HPP

#include "drako/math/mat4x4.hpp"
#include "drako/math/vector3.hpp"

#include "drako/graphics/vulkan_gpu_shader.hpp"

#include <vulkan/vulkan.hpp>

namespace drako::gpx::vulkan
{
    struct graphics_pipeline
    {
        vk::UniquePipeline       pipeline;
        vk::UniquePipelineLayout layout;
    };


    [[nodiscard]] bool check_required_features(vk::PhysicalDevice p) noexcept
    {
        const auto features = p.getFeatures();

        auto validation = true;
        // required for triangle edges rendering
        validation &= (features.fillModeNonSolid == vk::Bool32{ VK_TRUE });

        return validation;
    }

    [[nodiscard]] graphics_pipeline make_wireframe_pipeline(
        const vk::Device& device, const gpu_shader& vert, const vk::RenderPass& renderpass)
    {
        /*vvv create pipeline layout vvv*/
        const vk::DescriptorSetLayout descriptor_set_layouts[] = {
            {}
        };

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
            static_cast<uint32_t>(std::size(descriptor_set_layouts)), descriptor_set_layouts,
            static_cast<uint32_t>(std::size(push_constants)), push_constants
        };

        auto layout = device.createPipelineLayoutUnique(pipeline_layout_info);

        /*vvv create pipeline vvv*/

        const vk::PipelineShaderStageCreateInfo shader_stages[] = {
            { // vertex shader
                vk::PipelineShaderStageCreateFlags{},
                vk::ShaderStageFlagBits::eVertex,
                vert.shader_module(),
                vert.vertex_function_name(),
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
        const vk::PipelineViewportStateCreateInfo viewport_state{
            vk::PipelineViewportStateCreateFlags{},
            static_cast<uint32_t>(std::size(viewports)), viewports,
            static_cast<uint32_t>(std::size(scissors)), scissors
        };

        const vk::PipelineRasterizationStateCreateInfo rasterization_state{
            {},
            vk::Bool32{ VK_FALSE }, // depth clamping
            vk::Bool32{ VK_FALSE }, // rasterizer discard
            vk::PolygonMode::eLine,
            vk::CullModeFlagBits::eNone, // disable culling
            vk::FrontFace::eCounterClockwise,
            vk::Bool32{ VK_FALSE }, // depth bias
            {},                     // depth bias constant factor
            {},                     // depth bias clamp
            {},                     // depth bias slope factor
            1.f                     // line width
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
            vk::Bool32{ VK_TRUE },  // depth test
            vk::Bool32{ VK_FALSE }, // depth writing
            vk::CompareOp::eLess,
            vk::Bool32{ VK_FALSE }, // depth bounds test
            vk::Bool32{ VK_FALSE }, // stencil test
            {},
            {},
            {},
            {}
        };

        const vk::PipelineColorBlendAttachmentState color_blend_attachments[] = {
            // color buffer
            {
                vk::Bool32{ VK_FALSE }, // disable blending
                vk::BlendFactor{},      // not required
                vk::BlendFactor{},      // not required
                vk::BlendOp{},          // not required
                vk::BlendFactor{},      // not required
                vk::BlendFactor{},      // not required
                vk::BlendOp{},          // not required
                vk::ColorComponentFlagBits::eR |
                    vk::ColorComponentFlagBits::eG |
                    vk::ColorComponentFlagBits::eB }
        };
        const vk::PipelineColorBlendStateCreateInfo color_blend_state{
            {},
            vk::Bool32{ VK_FALSE },
            vk::LogicOp{}, // not required
            std::size(color_blend_attachments), color_blend_attachments,
            {} // not required
        };

        const vk::PipelineDynamicStateCreateInfo dynamic_state{
            {},
            0, static_cast<vk::DynamicState*>(nullptr)
        };

        const vk::GraphicsPipelineCreateInfo pipeline_info{
            vk::PipelineCreateFlags{},
            static_cast<uint32_t>(std::size(shader_stages)), shader_stages,
            &vertex_input_state,
            &input_assembly_state,
            static_cast<vk::PipelineTessellationStateCreateInfo*>(nullptr),
            &viewport_state,
            &rasterization_state,
            &multisample_state,
            &depth_stencil_state,
            &color_blend_state,
            &dynamic_state,
            layout.get(),
            renderpass,
            0,                       // subpass
            vk::Pipeline{ nullptr }, // base pipeline
            0                        // base pipeline index
        };
        auto pipeline = device.createGraphicsPipelineUnique(vk::PipelineCache{ nullptr }, pipeline_info);
        return { .pipeline = std::move(pipeline), .layout = std::move(layout) };
    }

} // namespace drako::gpx::vulkan

#endif // !DRAKO_VULKAN_WIREFRAME_PIPELINE_HPP