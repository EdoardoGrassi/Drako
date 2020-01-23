#pragma once
#ifndef DRAKO_BASIC_MESH_RENDERER_HPP
#define DRAKO_BASIC_MESH_RENDERER_HPP

#include "drako/graphics/vulkan_mesh.hpp"
#include "drako/graphics/vk_material.hpp"

#include "drako/math/matrix4x4.hpp"

namespace drako::gpx
{
    class basic_mesh_renderer
    {
    public:

        // Issue draw call for a single mesh.
        virtual void draw(const vulkan_mesh& m) noexcept = 0;

        // Issue draw call for a single indexed mesh.
        virtual void draw_indexed(const vulkan_mesh& m) noexcept = 0;

        // Issue a batched draw call.
        virtual void draw_batched(const mat4x4& vp,
            const std::vector<vulkan_mesh>& meshes,
            const std::vector<vk_gpu_shader>& shaders,
            const std::vector<mat4x4>& transforms) noexcept = 0;

        // virtual void execute_command_buffer() noexcept = 0;
    };
} // namespace drako::gpx

#endif // !DRAKO_BASIC_MESH_RENDERER_HPP