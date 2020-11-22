#pragma once
#ifndef DRAKO_SHADER_HPP
#define DRAKO_SHADER_HPP

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <vector>

namespace drako
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
        [[nodiscard]] std::string_view
        entry_point() const noexcept { return _entry; }

        friend std::istream& operator>>(std::istream&, shader_source&);
        friend std::ostream& operator<<(std::ostream&, const shader_source&);

        [[nodiscard]] const std::byte* data() const noexcept
        {
            return _source.data();
        }

        [[nodiscard]] size_t size() const noexcept
        {
            return _source.size();
        }

    private:
        std::vector<std::byte> _source;
        std::string            _entry;
    };

    shader_source::shader_source(shader_source&& other) noexcept
        : _source(std::move(other._source))
        , _entry(std::move(other._entry))
    {
    }

    shader_source& shader_source::operator=(shader_source&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            std::swap(_source, other._source);
            std::swap(_entry, other._entry);
        }
        return *this;
    }

    std::istream& operator>>(std::istream& is, shader_source& obj)
    {
        size_t size;
        is >> size;
        obj._source.resize(size);
        is.read(reinterpret_cast<char*>(obj._source.data()), size);
        // TODO: missing _entry serialization
        return is;
    }

    std::ostream& operator<<(std::ostream& os, const shader_source& obj)
    {
        os << std::size(obj._source);
        os.write(reinterpret_cast<const char*>(obj._source.data()), std::size(obj._source));
        // TODO: missing _entry serialization
        return os;
    }

    [[nodiscard]] shader_source make_from_stream(std::istream& is)
    {
        shader_source obj{};
        is >> obj;
        return std::move(obj);
    }


    [[nodiscard]] std::error_code
    compile_from_spirv(const std::filesystem::path& file, const std::filesystem::path& out) noexcept
    {
        std::error_code ec;
        if (const auto bytes = std::filesystem::file_size(file, ec); ec)
        {
            std::ifstream src(file, std::ios::binary);
            std::ofstream dst(out, std::ios::binary);
            dst << static_cast<uint32_t>(bytes) << src.rdbuf();
        }
    }
} // namespace drako

#endif // !DRAKO_SHADER_HPP