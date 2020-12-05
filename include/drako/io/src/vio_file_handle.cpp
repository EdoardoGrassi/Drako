#include "drako/io/vio_file_handle.hpp"

#include "drako/core/platform.hpp"

#if defined(_drako_platform_Win32)
#include <Windows.h>
#endif

#include <filesystem>
#include <system_error>

namespace drako::io::vectored
{
    constexpr file_handle::file_handle() noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
    }

    constexpr file_handle::file_handle(native_handle_type handle) noexcept
        : _handle{ handle }
    {
    }

    file_handle::file_handle(const path_type& file)
    {
        _handle = ::CreateFileW(file.wstring().data(),
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
        if (_handle == INVALID_HANDLE_VALUE)
            [[unlikely]] throw std::system_error(::GetLastError(), std::system_category());
    }

    file_handle::~file_handle() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
            if (::CloseHandle(_handle) == 0)
                [[unlikely]] std::terminate(); // can't throw in destructor
    }


    void file_handle::close()
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            if (::CloseHandle(_handle) == 0)
                [[unlikely]] throw std::system_error(::GetLastError(), std::system_category());
            _handle = INVALID_HANDLE_VALUE;
        }
    }

} // namespace drako::io::vectored