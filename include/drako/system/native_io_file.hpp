#pragma once
#ifndef DRAKO_NATIVE_IO_FILE_HPP
#define DRAKO_NATIVE_IO_FILE_HPP

#include <cstdint>
#include <filesystem>
#include <system_error>
#include <tuple>
#include <vector>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif


namespace drako::sys
{
    // Specifies the level of access permitted for a new file handle.
    // Combinations of different flags are accepted.
    // The access mode can't conflict with the sharing mode requested by other open handles of the same file.
    enum class access_mode : std::uint8_t
    {
        // Permits read operations on the file.
        read = (1 << 0),

        // Permits write operations on the file.
        write = (1 << 1),

        // Permits read and write operations on the file
        read_write = (read | write)
    };
    inline constexpr access_mode access_mode_read  = access_mode::read;
    inline constexpr access_mode access_mode_write = access_mode::write;
    inline constexpr access_mode access_mode_full  = access_mode::read_write;


    // Specifies the level of access permitted for a file that has already an open handle.
    // Combinations of different flags are accepted
    // File operations that don't meet sharing restrictions will always fail.
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


    struct vio_buffer_info
    {
        void* buffer;
    };

    using scatter_list = std::vector<vio_buffer_info>;
    using gather_list  = std::vector<vio_buffer_info>;


    // Handle to a platform-dependent file descriptor.
    class native_file
    {
    public:
        using path_type = std::filesystem::path;

        constexpr explicit native_file() noexcept;
        ~native_file() noexcept;

        native_file(const native_file&) noexcept = delete;
        native_file& operator=(const native_file&) noexcept = delete;

        constexpr native_file(native_file&&) noexcept;
        constexpr native_file& operator=(native_file&&) noexcept;

        DRAKO_NODISCARD std::error_code
        open(const path_type& p, access_mode a, share_mode s) noexcept;

        DRAKO_NODISCARD std::error_code
        close() noexcept;


        // Reads data from file to memory buffer.
        DRAKO_NODISCARD std::error_code
        read(void* dst, size_t bytes, size_t offset) noexcept;

        // Reads data from file to multiple memory buffers.
        DRAKO_NODISCARD std::error_code
        read_scatter(const scatter_list& dst, size_t bytes, size_t offset) noexcept;

        // Writes data from memory buffer to file.
        DRAKO_NODISCARD std::error_code
        write(void* src, size_t bytes, size_t offset) noexcept;

        // Writes data from multiple memory buffers to file.
        DRAKO_NODISCARD std::error_code
        write_gather(const gather_list& src, size_t bytes, size_t offset) noexcept;


#if defined(DRAKO_PLT_WIN32)
        DRAKO_FORCE_INLINE constexpr native_file(HANDLE win32_handle) noexcept
            : _handle(win32_handle)
        {
        }

        DRAKO_FORCE_INLINE constexpr operator HANDLE() const { return _handle; }
#endif

    private:
#if defined(DRAKO_PLT_WIN32)
        HANDLE _handle;

#elif defined(DRAKO_PLT_LINUX)
        int _handle; // Unix file descriptor

#endif
    };

} // namespace drako::sys

#endif // !DRAKO_NATIVE_IO_FILE_HPP