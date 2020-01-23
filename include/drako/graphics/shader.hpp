#pragma once
#ifndef DRAKO_SHADER_HPP
#define DRAKO_SHADER_HPP

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

namespace drako::gpx
{
    // Shader asset resource.
    class shader_source
    {
    public:
        explicit shader_source() noexcept = default;

        shader_source(const shader_source&) = delete;
        shader_source& operator=(const shader_source&) = delete;

        shader_source(shader_source&&) noexcept;
        shader_source& operator=(shader_source&&) noexcept;

        // Returns the identifier of the entry function of vertex stage.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr std::string_view
        entry_point() const noexcept { return _entry; }

        friend std::istream& operator>>(std::istream&, shader_source&);
        friend std::ostream& operator<<(std::ostream&, const shader_source&);

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr const std::byte*
        data() const noexcept
        {
            return _source.data();
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t
        size() const noexcept
        {
            return _source.size();
        }

    private:
        std::vector<std::byte> _source;
        std::string            _entry;
    };

    inline constexpr shader_source::shader_source(shader_source&& other) noexcept
        : _source(std::move(other._source))
        , _entry(std::move(other._entry))
    {
    }

    inline constexpr shader_source& shader_source::operator=(shader_source&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            std::swap(_source, other._source);
            std::swap(_entry, other._entry);
        }
        return *this;
    }

    inline std::istream& operator>>(std::istream& is, shader_source& obj)
    {
        is >> obj._size;
        is.read(reinterpret_cast<char*>(obj._data), obj._size);
        return is;
    }

    inline std::ostream& operator<<(std::ostream& os, const shader_source& obj)
    {
        os << obj._size;
        os.write(reinterpret_cast<char*>(obj._data), obj._size);
        return os;
    }

    DRAKO_NODISCARD inline shader_source make_from_stream(std::istream& is)
    {
        shader_source obj{};
        is >> obj;
        return std::move(obj);
    }

    DRAKO_NODISCARD
    inline std::error_code compile_from_spirv(const std::filesystem::path& file, const std::filesystem::path& out) noexcept
    {
        std::error_code ec;
        if (const auto bytes = std::filesystem::file_size(file, ec); ec)
        {
            std::ifstream src(file, std::ios::binary);
            std::ofstream dst(out, std::ios::binary);
            dst << static_cast<uint32_t>(bytes) << src.rdbuf();
        }
    }
} // namespace drako::gpx

#endif // !DRAKO_SHADER_HPP