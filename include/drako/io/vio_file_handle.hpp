#pragma once
#ifndef DRAKO_VIO_FILE_HANDLE_HPP
#define DRAKO_VIO_FILE_HANDLE_HPP

#include "drako/core/preprocessor/platform_macros.hpp"

#if defined(_drako_platform_Win32)
#include <Windows.h>
#endif

#include <filesystem>

namespace drako::io::vectored
{
    /// @brief Handle that supports vectorized I/O on a file.
    class file_handle
    {
    public:
        using path_type = std::filesystem::path;
#if defined(_drako_platform_Win32)
        using native_handle_type = HANDLE;
#endif
        explicit constexpr file_handle() noexcept;
        explicit constexpr file_handle(native_handle_type) noexcept;
        explicit file_handle(const path_type&);
        ~file_handle() noexcept;

        file_handle(const file_handle&) = delete;
        file_handle& operator=(const file_handle&) = delete;

        file_handle(file_handle&&) noexcept;
        file_handle operator=(file_handle&&) noexcept;

        void close();

        void scatter();

        void gather();

        [[nodiscard]] native_handle_type native_handle() noexcept;

    private:
        native_handle_type _handle;
    };
} // namespace drako::io

#endif // !DRAKO_VIO_FILE_HANDLE_HPP