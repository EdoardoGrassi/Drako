#pragma once
#ifndef DRAKO_INPUT_OUTPUT_FILE_HANDLE_HPP
#define DRAKO_INPUT_OUTPUT_FILE_HANDLE_HPP

#include "drako/core/platform.hpp"
#include "drako/io/core.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <cstdint>
#include <filesystem>
#include <system_error>
#include <tuple>
#include <vector>

#include <fstream>

namespace drako::io
{


    // Handle to a platform-dependent file descriptor.
    class input_UniqueOutputFile
    {
    public:
        using path_type = std::filesystem::path;

        explicit constexpr input_UniqueOutputFile() noexcept = default;

        explicit constexpr input_UniqueOutputFile(native_handle_type handle) noexcept
            : _handle{ handle } {}

        explicit input_UniqueOutputFile(const path_type& filename, creation c)
            : _handle{ io::open(filename, mode::read_write, c) } {}

        ~input_UniqueOutputFile() noexcept
        {
            if (_handle != INVALID_HANDLE_VALUE)
                io::close(_handle);
        }

        input_UniqueOutputFile(const input_UniqueOutputFile&) noexcept = delete;
        input_UniqueOutputFile& operator=(const input_UniqueOutputFile&) noexcept = delete;

        input_UniqueOutputFile(input_UniqueOutputFile&&) noexcept;
        input_UniqueOutputFile& operator=(input_UniqueOutputFile&&) noexcept;

        void close();

        // Reads data from file to memory buffer.
        void read(void* dst, std::size_t bytes, std::size_t offset);

        // Writes data from memory buffer to file.
        void write(void* src, std::size_t bytes, std::size_t offset);

        [[nodiscard]] native_handle_type native_handle() const noexcept { return _handle; }

    private:
        native_handle_type _handle = INVALID_HANDLE_VALUE;
    };

} // namespace drako::io

#endif // !DRAKO_INPUT_OUTPUT_FILE_HANDLE_HPP