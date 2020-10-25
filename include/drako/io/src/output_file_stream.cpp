#include "drako/io/output_file_stream.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/io/io_api_defs.hpp"

#if defined(_drako_platform_Win32)
#include <Windows.h>
#endif

#include <filesystem>

namespace drako::io
{
    constexpr output_file_stream::output_file_stream() noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
    }

    constexpr output_file_stream::output_file_stream(native_handle_type handle) noexcept
        : _handle{ handle }
    {
    }

    output_file_stream::output_file_stream(const path_type& file, creation c = creation::if_needed)
    {
        DWORD open_mode = OPEN_ALWAYS;
        if (c == creation::open_existing)
            open_mode = OPEN_EXISTING;
        if (c == creation::truncate_existing)
            open_mode = TRUNCATE_EXISTING;

        _handle = ::CreateFileW(file.wstring().data(),
            GENERIC_READ, 0, NULL, open_mode, FILE_ATTRIBUTE_NORMAL, NULL);
        if (_handle == INVALID_HANDLE_VALUE)
            throw std::system_error(::GetLastError(), std::system_category());
    }

    output_file_stream::output_file_stream(output_file_stream&& other) noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
        using std::swap;
        swap(_handle, other._handle);
    }

    output_file_stream& output_file_stream::operator=(output_file_stream&& other) noexcept
    {
        using std::swap;
        swap(_handle, other._handle);
        return *this;
    }

    output_file_stream::~output_file_stream() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
            if (::CloseHandle(_handle) == 0)
                [[unlikely]] std::terminate(); // can't throw in destructor
    }

    void output_file_stream::close()
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            if (::CloseHandle(_handle) == 0)
                [[unlikely]] throw std::system_error(::GetLastError(), std::system_category());
            _handle = INVALID_HANDLE_VALUE;
        }
    }

    void output_file_stream::write(const std::byte* src, size_t bytes)
    {
        DWORD written_bytes;
        if (::WriteFile(_handle, src, bytes, &written_bytes, NULL) == FALSE)
            throw std::system_error(::GetLastError(), std::system_category());
    }
} // namespace drako::io