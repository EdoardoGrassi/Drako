#include "drako/io/input_file_stream.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <filesystem>

namespace drako::io
{
    constexpr input_file_stream::input_file_stream() noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
    }

    constexpr input_file_stream::input_file_stream(native_handle_type handle) noexcept
        : _handle{ handle }
    {
    }

    input_file_stream::input_file_stream(const path_type& file)
    {
        _handle = ::CreateFileW(file.wstring().data(),
            GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (_handle == INVALID_HANDLE_VALUE)
            throw std::system_error(::GetLastError(), std::system_category());
    }

    input_file_stream::input_file_stream(input_file_stream&& other) noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
        using std::swap;
        swap(_handle, other._handle);
    }

    input_file_stream& input_file_stream::operator=(input_file_stream&& other) noexcept
    {
        using std::swap;
        swap(_handle, other._handle);
        return *this;
    }

    input_file_stream::~input_file_stream() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
            if (::CloseHandle(_handle) == 0)
                std::terminate(); // can't throw in destructor
    }

    void input_file_stream::close()
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            if (::CloseHandle(_handle) == 0)
                throw std::system_error(::GetLastError(), std::system_category());
            _handle = INVALID_HANDLE_VALUE;
        }
    }

    void input_file_stream::read(std::byte* dst, size_t bytes)
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

    input_file_stream::native_handle_type input_file_stream::native_handle() noexcept
    {
        return _handle;
    }
} // namespace drako::io