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
        undefined = 0
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

        [[nodiscard]] inline constexpr uint16_t width() const noexcept;
        [[nodiscard]] inline constexpr uint16_t height() const noexcept;
        [[nodiscard]] inline constexpr uint16_t depth() const noexcept;
    };

    // Contains structural information shared by all types of textures.
    struct texture_desc
    {
        uint16_t       width         = 0;
        uint16_t       height        = 0;
        uint16_t       depth         = 0;
        uint8_t        stride        = 0;
        texture_format format        = texture_format::undefined;
        uint8_t        mipmap_levels = 0;
    };

    class texture_1d
    {
    };


    struct texture_2d_descriptor
    {
        uint16_t       width         = 0;
        uint16_t       height        = 0;
        texture_format format        = texture_format::undefined;
        uint8_t        mipmap_levels = 0;
    };

    template <typename Alloc>
    class texture_2d : private basic_texture
    {
    public:
        explicit texture_2d(const texture_desc& desc, Alloc& al = Alloc())
            : _alloc(al)
            , _size(0)
            , _data(nullptr)
        {
        }

        explicit texture_2d(const texture_desc& d, const std::byte* data, Alloc& al = Alloc())
            : _alloc(al)
            , _size(d.width * d.height * d.depth * d.stride)
            , _data(std::allocator_traits<Alloc>::allocate(a, _size))
        {
            DRAKO_ASSERT(data != nullptr);

            if (data == nullptr)
                std::exit(EXIT_FAILURE); // TODO: will never happen, std::allocator_traits<>::allocate throws on failure

            std::memcpy(_data, data, _size);
        }

        texture_2d(const texture_2d& rhs);
        texture_2d& operator=(const texture_2d& rhs);

        texture_2d(texture_2d&& rhs);
        texture_2d& operator=(texture_2d&& rhs);

        [[nodiscard]] constexpr auto data() const noexcept { return _data; }
        [[nodiscard]] constexpr auto   width() const noexcept { return _width; }
        [[nodiscard]] constexpr auto   height() const noexcept { return _height; }
        [[nodiscard]] constexpr auto   mipmap_levels() const noexcept { return _mipmap_levels; }

    private:
        Alloc&     _alloc;
        size_t     _size;
        std::byte* _data;
        uint16_t   _width;
        uint16_t   _height;
        uint8_t    _mipmap_levels;
    };

    class texture_2d_view
    {
    public:
        explicit constexpr texture_2d_view() noexcept
            : _meta{}
            , _data{ nullptr }
        {
        }

        explicit constexpr texture_2d_view(const texture_2d_descriptor& meta, const std::byte* data) noexcept
            : _meta{ meta }
            , _data{ data }
        {
        }

        explicit constexpr texture_2d_view(const texture_2d_view& other) noexcept = default;

        [[nodiscard]] constexpr const std::byte* data() const noexcept;

        [[nodiscard]] constexpr const texture_2d_descriptor& meta() const noexcept
        {
            return _meta;
        }

    private:
        texture_2d_descriptor _meta;
        const std::byte*      _data;
    };


    struct texture_3d_descriptor
    {
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
        texture_desc     _meta;
        const std::byte* _data;
    };

    class texture_view
    {
    public:
        explicit constexpr texture_view() noexcept
            : _meta{}
            , _data{ nullptr }
        {
        }

        [[nodiscard]] DRAKO_FORCE_INLINE const std::byte* data() const noexcept
        {
            return _data;
        }

        [[nodiscard]] DRAKO_FORCE_INLINE constexpr const texture_desc& meta() const noexcept
        {
            return _meta;
        }

    private:
        texture_desc     _meta;
        const std::byte* _data;
    };


    [[nodiscard]] DRAKO_FORCE_INLINE constexpr uint32_t
    width(const texture_view& t) noexcept
    {
        return t.meta().width;
    }

    [[nodiscard]] DRAKO_FORCE_INLINE constexpr uint32_t
    width(const texture_2d_view& t) noexcept
    {
        return t.meta().width;
    }

    [[nodiscard]] DRAKO_FORCE_INLINE constexpr uint32_t
    height(const texture_view& t) noexcept
    {
        return t.meta().height;
    }

    [[nodiscard]] DRAKO_FORCE_INLINE constexpr uint32_t
    height(const texture_2d_view& t) noexcept
    {
        return t.meta().height;
    }

    [[nodiscard]] DRAKO_FORCE_INLINE constexpr uint32_t
    mipmap_levels(const texture_view& t) noexcept
    {
        return t.meta().mipmap_levels;
    }

    [[nodiscard]] DRAKO_FORCE_INLINE constexpr uint32_t
    mipmap_levels(const texture_2d_view& t) noexcept
    {
        return t.meta().mipmap_levels;
    }

} // namespace drako

#endif // !DRAKO_TEXTURE_TYPES_HPP