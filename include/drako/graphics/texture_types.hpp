#pragma once
#ifndef DRAKO_TEXTURE_TYPES_HPP
#define DRAKO_TEXTURE_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <memory>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"

namespace drako
{
    // Uniquely identifies a texture resource.
    enum class texture_guid : std::uint32_t;

    enum class texture_format : std::uint8_t
    {
#if defined(DRAKO_USE_VULKAN)

#endif
    };

    // Base class for texture assets.
    class basic_texture
    {
    public:
        enum class dimensionality
        {
            ANY,
            TEXTURE_1D,
            TEXTURE_2D,
            TEXTURE_3D,
            ARRAY_2D,
            ARRAY_3D
        };

        enum class filter_mode
        {
        };

        enum class color_format
        {
        };

        enum class anisotropic_filter_mode
        {
        };

        enum class sampler_wrap_mode
        {
            REPEAT,
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            MIRRORED_CLAMP_TO_EDGE,
            SOLID_COLOR
        };

        DRAKO_NODISCARD inline constexpr uint16_t width() const noexcept;
        DRAKO_NODISCARD inline constexpr uint16_t height() const noexcept;
        DRAKO_NODISCARD inline constexpr uint16_t depth() const noexcept;
    };

    // Contains structural information shared by all types of textures.
    struct texture_desc
    {
        uint16_t       width;
        uint16_t       height;
        texture_format format;
        uint8_t        stride;
    };

    struct texture_2d_desc
    {
    };

    class texture_1d
    {
    public:
    private:
    };

    template <typename Alloc>
    class texture_2d : private basic_texture
    {
    public:
        explicit texture_2d(const texture_desc& desc, Alloc& al = Alloc())
            : _alloc(al)
            , _data(nullptr)
        {
        }

        explicit texture_2d(const texture_desc& d, const std::byte* data, Alloc& al = Alloc())
            : _alloc(al)
            , _data(std::allocator_traits<Alloc>::allocate(a, d.width * d.height * d.stride))
        {
            DRAKO_ASSERT(data != nullptr);

            if (data == nullptr)
                std::exit(EXIT_FAILURE); // TODO: will never happen, std::allocator_traits<>::allocate throws on failure

            std::memcpy(_data, data, d.width * d.height * d.stride);
        }

        texture_2d(const texture_2d& rhs);
        texture_2d& operator=(const texture_2d& rhs);

        texture_2d(texture_2d&& rhs);
        texture_2d& operator=(texture_2d&& rhs);

        DRAKO_NODISCARD constexpr auto data() const noexcept { return _data; }
        DRAKO_NODISCARD constexpr auto width() const noexcept { return _width; }
        DRAKO_NODISCARD constexpr auto height() const noexcept { return _height; }
        DRAKO_NODISCARD constexpr auto mipmap_levels() const noexcept { return _mipmap_levels; }

    private:
        Alloc&     _alloc;
        std::byte* _data;
        uint16_t   _width;
        uint16_t   _height;
        uint8_t    _mipmap_levels;
    };

    class texture_2d_view
    {
    public:
        explicit constexpr texture_2d_view() noexcept
            : _desc{}
            , _data{ nullptr }
        {
        }

        explicit constexpr texture_2d_view(const texture_2d_desc& d, const std::byte* data) noexcept
            : _desc{ d }
            , _data{ data }
        {
        }

        explicit constexpr texture_2d_view(const texture_2d_view& other) noexcept = default;

        DRAKO_NODISCARD constexpr const std::byte* data() const noexcept;

    private:
        texture_2d_desc  _desc;
        const std::byte* _data;
    };


    template <typename Allocator>
    class texture_3d : private basic_texture
    {
    public:
    private:
    };

    class texture_3d_view
    {
    };



    template <typename Allocator>
    class texture
    {
    public:
        explicit texture() noexcept;

        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;

        constexpr texture(texture&&) noexcept;
        constexpr texture& operator=(texture&&) noexcept;

    private:
        texture_desc     _desc;
        const std::byte* _data;
    };

    class texture_view
    {
    public:
        explicit constexpr texture_view() noexcept
            : _desc{}
            , _data{ nullptr }
        {
        }

        DRAKO_NODISCARD uint16_t width() const noexcept;

        DRAKO_NODISCARD uint16_t height() const noexcept;

        DRAKO_NODISCARD uint16_t depth() const noexcept;

    private:
        texture_desc     _desc;
        const std::byte* _data;
    };

} // namespace drako

#endif // !DRAKO_TEXTURE_TYPES_HPP