#pragma once
#ifndef DRAKO_VULKAN_RENDER_ENGINE_HPP
#define DRAKO_VULKAN_RENDER_ENGINE_HPP

#include "drako/graphics/vulkan_runtime_context.hpp"

namespace drako::vulkan
{
    class RenderEngine
    {
    public:
        explicit RenderEngine(const context& ctx);
    private:
    };
} // namespace drako::vulkan

#endif // !DRAKO_VULKAN_RENDER_ENGINE_HPP