#pragma once
#ifndef DRAKO_AIO_FILE_HANDLE_HPP
#define DRAKO_AIO_FILE_HANDLE_HPP

#include "drako/core/platform.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <filesystem>


namespace drako::io::async
{

    /// @brief Stream that supports asynchronous operations.
    class file_handle
    {
    public:
        using path_type = std::filesystem::path;
#if defined(DRAKO_PLT_WIN32)
        using native_handle_type = HANDLE; // win32 file handle
#else
#error Not supported on current platform.
#endif

        constexpr explicit file_handle() noexcept;
        constexpr explicit file_handle(native_handle_type) noexcept;
        explicit file_handle(const path_type& file);

        file_handle(const file_handle&) noexcept = delete;
        file_handle& operator=(const file_handle&) noexcept = delete;

        file_handle(file_handle&&) noexcept;
        file_handle& operator=(file_handle&&) noexcept;

        ~file_handle() noexcept;

        void close();

        // Reads data from file to memory buffer.
        void read(std::byte* dst, std::size_t bytes) noexcept;

        // Writes data from memory buffer to file.
        void write(const std::byte* src, std::size_t bytes) noexcept;

        [[nodiscard]] native_handle_type native_handle() const noexcept;

    private:
        native_handle_type _handle;
    };
} // namespace drako::io::async

#endif // !DRAKO_AIO_FILE_HANDLE_HPP