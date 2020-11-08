#include "drako/io/input_file_stream.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <filesystem>

namespace drako::io
{
    using _this = input_file_stream;

    constexpr _this::input_file_stream() noexcept
        : _handle{ INVALID_HANDLE_VALUE } {}

    constexpr _this::input_file_stream(native_handle_type handle) noexcept
        : _handle{ handle } {}

    _this::input_file_stream(const path_type& file)
    {
        _handle = ::CreateFileW(file.wstring().data(),
            GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (_handle == INVALID_HANDLE_VALUE)
            throw std::system_error(::GetLastError(), std::system_category());
    }

    _this::input_file_stream(_this&& other) noexcept
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

    _this::~input_file_stream() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
            if (::CloseHandle(_handle) == 0)
                std::terminate(); // can't throw in destructor
    }

    void _this::close()
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            if (::CloseHandle(_handle) == 0)
                throw std::system_error(::GetLastError(), std::system_category());
            _handle = INVALID_HANDLE_VALUE;
        }
    }

    void _this::read(std::byte* dst, size_t bytes)
    {
        while (bytes > 0)
        {
            DWORD read_bytes;
            if (::ReadFile(_handle, dst, bytes, &read_bytes, NULL) == FALSE)
                throw std::system_error(::GetLastError(), std::system_category());

            dst += read_bytes;
            bytes -= read_bytes;
        }
    }

    _this::native_handle_type _this::native_handle() noexcept
    {
        return _handle;
    }

} // namespace drako::io