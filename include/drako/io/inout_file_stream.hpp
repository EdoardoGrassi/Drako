#pragma once
#ifndef DRAKO_INOUT_FILE_STREAM_HPP
#define DRAKO_INOUT_FILE_STREAM_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/io/io_api_defs.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <cstdint>
#include <filesystem>
#include <system_error>
#include <tuple>
#include <vector>


namespace drako::io
{


    // Handle to a platform-dependent file descriptor.
    class inout_file_stream
    {
    public:
        using path_type = std::filesystem::path;
#if defined(_drako_platform_Win32)
        using native_handle_type = HANDLE; // win32 file handle

#elif defined(_drako_platform_Linux)
        using native_handle_type = int; // Unix file handle
#endif

        constexpr explicit inout_file_stream() noexcept;
        constexpr explicit inout_file_stream(native_handle_type) noexcept;
        explicit inout_file_stream(const path_type& file, creation c);
        ~inout_file_stream() noexcept;

        inout_file_stream(const inout_file_stream&) noexcept = delete;
        inout_file_stream& operator=(const inout_file_stream&) noexcept = delete;

        inout_file_stream(inout_file_stream&&) noexcept;
        inout_file_stream& operator=(inout_file_stream&&) noexcept;

        void close();

        // Reads data from file to memory buffer.
        void read(void* dst, size_t bytes, size_t offset);

        // Writes data from memory buffer to file.
        void write(void* src, size_t bytes, size_t offset);

        [[nodiscard]] native_handle_type native_handle() const noexcept { return _handle; }

    private:
        native_handle_type _handle;
    };

} // namespace drako::io

#endif // !DRAKO_INOUT_FILE_STREAM_HPP