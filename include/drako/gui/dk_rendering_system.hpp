#pragma once
#ifndef DRAKO_GUI_RENDERING_SYSTEM_HPP
#define DRAKO_GUI_RENDERING_SYSTEM_HPP

#include <vector>

#include "drako/dod/dk_data_table.hpp"
#include "drako/jobs/job_api.hpp"

#include "drako/gui/dk_widget.hpp"

#include "drako/graphics/vk_material.hpp"
#include "drako/graphics/vk_shader.hpp"
#include "drako/graphics/vk_gui_renderer.hpp"

#include "drako/math/aarect.hpp"

namespace drako
{
    // using namespace drako::graphics;

    template<typename Resource>
    using Handle = uint32_t;

    // Performs rendering on target OS window.
    //
    class RenderingEngine
    {
    public:

        constexpr explicit RenderingEngine(size_t reserved_slots) noexcept;
        ~RenderingEngine() noexcept;

        void insert(WidgetGUID id, Handle<AARect> t, Handle<Shader> s, Handle<Material> m) noexcept;

        void remove(WidgetGUID id) noexcept;

        void render() const noexcept;

    private:

        std::vector<Handle<WidgetGUID>>     _widgets;
        std::vector<Handle<Shader>>         _shaders;
        std::vector<Handle<Material>>       _materials;
        const std::vector<Handle<AARect>>&  _transforms;
        vk_gui_renderer                     _renderer;
    };


    void RenderingEngine::render() const noexcept
    {
        for (const auto& rect : _transforms)
        {

        }
    }

} // namespace drako::gui

#endif // !DRAKO_GUI_RENDERING_SYSTEM_HPP