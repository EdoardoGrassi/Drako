#include "drako/io/output_file_stream.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/io/io_api_defs.hpp"

#if defined(_drako_platform_Win32)
#include <Windows.h>
#endif

#include <filesystem>

namespace drako::io
{
    using _this = output_file_stream;

    constexpr _this::output_file_stream() noexcept
        : _handle{ INVALID_HANDLE_VALUE } {}

    constexpr _this::output_file_stream(native_handle_type handle) noexcept
        : _handle{ handle } {}

    _this::output_file_stream(const path_type& file, const creation c)
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

    _this::output_file_stream(_this&& other) noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
        using std::swap;
        swap(_handle, other._handle);
    }

    _this& _this::operator=(_this&& other) noexcept
    {
        using std::swap;
        swap(_handle, other._handle);
        return *this;
    }

    _this::~output_file_stream() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
            if (::CloseHandle(_handle) == 0)
                [[unlikely]] std::terminate(); // can't throw in destructor
    }

    void _this::close()
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            if (::CloseHandle(_handle) == 0)
                [[unlikely]] throw std::system_error(::GetLastError(), std::system_category());
            _handle = INVALID_HANDLE_VALUE;
        }
    }

    void _this::write(const std::byte* src, size_t bytes)
    {
        if (bytes == 0)
            return;

        DWORD written_bytes;
        if (::WriteFile(_handle, src, bytes, &written_bytes, NULL) == FALSE)
            throw std::system_error(::GetLastError(), std::system_category());
    }

    [[nodiscard]] _this::native_handle_type _this::native_handle() noexcept
    {
        return _handle;
    }

} // namespace drako::io