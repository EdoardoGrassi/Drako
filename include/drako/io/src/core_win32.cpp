#include "drako/io/core.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"

#if !defined(DRAKO_PLT_WIN32)
#error This source file shouldn't be included.
#endif

#include <Windows.h>

#include <cassert>
#include <system_error>

namespace drako::io
{
    using _ec = std::error_code;
    using _se = std::system_error;

    handle open(const path_type& filename, mode m, creation c)
    {
        DWORD mode = 0;
        if (m == mode::read)
            mode |= GENERIC_READ;
        if (m == mode::write)
            mode |= GENERIC_WRITE;
        if (m == mode::read_write)
            mode |= GENERIC_READ | GENERIC_WRITE;

        DWORD create = OPEN_ALWAYS;
        if (c == creation::open_existing)
            create = OPEN_EXISTING;
        if (c == creation::truncate_existing)
            create = TRUNCATE_EXISTING;

        const auto h = ::CreateFileW(filename.wstring().data(),
            mode, 0, NULL, create, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h == INVALID_HANDLE_VALUE)
            [[unlikely]] throw _se(::GetLastError(), std::system_category());
        return h;
    }

    void read(handle src, std::byte* dst, std::size_t bytes)
    {
        assert(src != INVALID_HANDLE_VALUE);
        assert(dst);
        while (bytes > 0)
        { // ReadFile() may not read all the requested bytes so we have to loop
            DWORD read_bytes;
            if (::ReadFile(src, dst, bytes, &read_bytes, NULL) == FALSE)
                [[unlikely]] throw _se(::GetLastError(), std::system_category());

            dst += read_bytes;
            bytes -= read_bytes;
        }
    }

    void read(handle src, std::byte* dst, std::size_t bytes, _ec& ec) noexcept
    {
        assert(src != INVALID_HANDLE_VALUE);
        assert(dst);
        while (bytes > 0)
        { // ReadFile() may not read all the requested bytes so we have to loop
            DWORD read_bytes;
            if (::ReadFile(src, dst, bytes, &read_bytes, NULL) == FALSE)
            {
                [[unlikely]] ec.assign(::GetLastError(), std::system_category());
                return;
            }

            dst += read_bytes;
            bytes -= read_bytes;
        }
        ec.clear();
    }

    void read(handle src, std::byte* dst, std::size_t bytes, std::size_t offset)
    {
        assert(src != INVALID_HANDLE_VALUE);
        assert(dst);
        while (bytes > 0)
        { // ReadFile() may not read all the requested bytes so we have to loop
            using _off = decltype(OVERLAPPED::Offset);

            OVERLAPPED ov = {};
            ov.Offset     = static_cast<_off>(offset);
            ov.OffsetHigh = static_cast<_off>(offset >> (sizeof(_off) * 8));

            DWORD read_bytes;
            if (::ReadFile(src, dst, bytes, &read_bytes, &ov) == FALSE)
                [[unlikely]] throw _se(::GetLastError(), std::system_category());

            dst += read_bytes;
            offset += read_bytes;
            bytes -= read_bytes;
        }
    }

    void read(handle src, std::byte* dst, std::size_t bytes, std::size_t offset, _ec& ec) noexcept
    {
        assert(src != INVALID_HANDLE_VALUE);
        assert(dst);
        while (bytes > 0)
        { // ReadFile() may not read all the requested bytes so we have to loop
            using _off = decltype(OVERLAPPED::Offset);

            OVERLAPPED ov = {};
            ov.Offset     = static_cast<_off>(offset);
            ov.OffsetHigh = static_cast<_off>(offset >> (sizeof(_off) * 8));

            DWORD read_bytes;
            if (::ReadFile(src, dst, bytes, &read_bytes, &ov) == FALSE)
            {
                [[unlikely]] ec.assign(::GetLastError(), std::system_category());
                return;
            }

            dst += read_bytes;
            offset += read_bytes;
            bytes -= read_bytes;
        }
        ec.clear();
    }

    void write(const std::byte* src, handle dst, std::size_t bytes)
    {
        assert(src);
        assert(dst != INVALID_HANDLE_VALUE);
        if (bytes == 0)
            return;
        DWORD written_bytes; // discarded
        if (::WriteFile(dst, src, bytes, &written_bytes, NULL) == FALSE)
            [[unlikely]] throw _se(::GetLastError(), std::system_category());
    }

    void write(const std::byte* src, handle dst, std::size_t bytes, _ec& ec) noexcept
    {
        assert(src);
        assert(dst != INVALID_HANDLE_VALUE);
        if (bytes == 0)
            return;
        [[maybe_unused]] DWORD written_bytes; // discarded
        if (::WriteFile(dst, src, bytes, &written_bytes, NULL) == FALSE)
            [[unlikely]] ec.assign(::GetLastError(), std::system_category());
        else
            ec.clear();
    }

    void flush(handle h)
    {
        assert(h != INVALID_HANDLE_VALUE);
        ::FlushFileBuffers(h);
    }

    void close(handle h)
    {
        if (::CloseHandle(h) == 0)
            [[unlikely]] throw _se(::GetLastError(), std::system_category());
    }

} // namespace drako::io