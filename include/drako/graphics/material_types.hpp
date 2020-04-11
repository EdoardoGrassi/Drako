#pragma once
#ifndef DRAKO_MATERIAL_TYPES_HPP_
#define DRAKO_MATERIAL_TYPES_HPP_

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/graphics/shader_types.hpp"
#include "drako/system/memory_stream.hpp"

namespace drako::gpx
{
    using material_template_id = std::uint16_t;

    enum class material_data_type : std::uint8_t
    {
        inline_data,
        texture_1d,
        texture_2d,
        texture_3d,
        sampler_texture_1d,
        sampler_texture_2d,
        sampler_texture_3d,
    };

    // Describes the resource binding with the shader.
    struct material_data_binding
    {
        std::uint8_t       shader_set_attribute;     // layout set number referenced in shader code
        std::uint8_t       shader_binding_attribute; // layout binding number referenced in shader code
        std::uint8_t       shader_resource_count;    // array size
        material_data_type shader_resource_type;     // type of resource
    };
    static_assert(sizeof(material_data_binding) == 4,
        "Bad class layout: wasted internal padding bytes");


    // Describes the blueprint layout of a class of material instances.
    class material_template
    {
    public:
        explicit constexpr material_template(const std::vector<material_data_binding>& bindings);

        template <typename Allocator>
        explicit material_template(memory_stream<Allocator>& ms);

        friend std::istream& operator>>(std::istream&, material_template&);
        friend std::ostream& operator<<(std::ostream&, const material_template&);

        shader_source& vert_shader;
        shader_source& frag_shader;
        //shader_source& geom_shader;
        //shader_source& tesc_shader;

        std::vector<material_data_binding> bindings;

    private:
        std::vector<std::uint16_t> _props_ids;    // properties IDs
        std::vector<std::string>   _props_names;  // properties names
        std::vector<std::uint16_t> _props_offset; // properties offset in data block
    };


    // Material data used by the rendering system.
    struct material_instance
    {
    public:
        explicit material_instance(material_template_id id, void* data, size_t bytes) noexcept;

        /*
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr const void* data() const noexcept
        {
            return _blob.data();
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t size() const noexcept
        {
            return _blob.size();
        }
        */

        friend std::istream& operator>>(std::istream&, material_instance&);
        friend std::ostream& operator<<(std::ostream&, const material_instance&);

        std::byte*  data;
        std::size_t size;
    };

} // namespace drako::gpx

#endif // !DRAKO_MATERIAL_TYPES_HPP_