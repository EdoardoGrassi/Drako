#pragma once
#ifndef DRAKO_IO_CORE_HPP
#define DRAKO_IO_CORE_HPP

#include "drako/core/preprocessor/platform_macros.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <cstddef>
#include <filesystem>

namespace drako::io
{
    using path_type = std::filesystem::path;
#if defined(DRAKO_PLT_WIN32)
    using native_handle_type = HANDLE; // win32 file handle
#else
#error Not supported on current platform.
#endif

    using handle = native_handle_type;


    /// @brief Allowed file operations.
    /// @note Combinations of different flags are accepted.
    enum class mode : std::uint8_t
    {
        /// @brief Only read operations.
        read = (1 << 0),

        /// @brief Only write operations.
        write = (1 << 1),

        /// @brief Both read and write operations.
        read_write = (read | write)
    };


    /// @brief File sharing mode.
    /// @note Combinations of different flags are accepted.
    enum class share_mode : std::uint8_t
    {
        // Prevents sharing of the file.
        exclusive = 0,

        // Share read permission.
        read = (1 << 0),

        // Share write permission.
        write = (1 << 1),

        // Share read and write permission.
        read_write = (read | write),

        // Share delete permission.
        deletable = (1 << 2),

        // Share permission with child processes that inherits the handle.
        inheritable = (1 << 3)
    };

    /// @brief Action performed when opening a file.
    enum class creation
    {
        /// @brief Open only already existing files.
        open_existing,

        /// @brief Create file if doesn't exist.
        if_needed,

        /// @brief Truncate only already existing files.
        truncate_existing
    };



    [[nodiscard]] handle open(const path_type& filename, mode m, creation c);

    void read(handle src, std::byte* dst, std::size_t bytes);
    void read(handle src, std::byte* dst, std::size_t bytes, std::size_t offset);

    void read(handle src, std::byte* dst, std::size_t bytes, std::error_code& ec) noexcept;
    void read(handle src, std::byte* dst, std::size_t bytes, std::size_t offset, std::error_code& ec) noexcept;

    void write(const std::byte* src, handle dst, std::size_t bytes);
    void write(const std::byte* src, handle dst, std::size_t bytes, std::size_t offset);

    void write(const std::byte* src, handle dst, std::size_t bytes, std::error_code& ec) noexcept;
    void write(const std::byte* src, handle dst, std::size_t bytes, std::size_t offset, std::error_code& ec) noexcept;

    void flush(handle h);

    void close(handle h);


} // namespace drako::io

#endif // !DRAKO_IO_CORE_HPP