#include "drako/io/core.hpp"

#include "drako/core/platform.hpp"

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

    Handle open(const path_type& filename, Mode m, Create c)
    {
        DWORD mode = 0;
        if (m == Mode::read)
            mode |= GENERIC_READ;
        if (m == Mode::write)
            mode |= GENERIC_WRITE;
        if (m == Mode::read_write)
            mode |= GENERIC_READ | GENERIC_WRITE;

        DWORD create = CREATE_ALWAYS;
        if (c == Create::open_existing)
            create = OPEN_EXISTING;
        if (c == Create::truncate_existing)
            create = TRUNCATE_EXISTING;
        if (c == Create::require_new)
            create = CREATE_NEW;

        const auto h = ::CreateFileW(filename.wstring().data(),
            mode, 0, NULL, create, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h == INVALID_HANDLE_VALUE)
            [[unlikely]] throw _se(::GetLastError(), std::system_category());
        return h;
    }

    std::size_t read(Handle src, std::byte* dst, std::size_t bytes)
    {
        assert(src != INVALID_HANDLE_VALUE);
        assert(dst);

        if (bytes == 0)
            [[unlikely]] return 0;

        DWORD read_bytes;
        if (::ReadFile(src, dst, bytes, &read_bytes, NULL) == FALSE)
            [[unlikely]] throw _se(::GetLastError(), std::system_category());

        return read_bytes;
    }

    std::size_t read(Handle src, std::byte* dst, std::size_t bytes, _ec& ec) noexcept
    {
        assert(src != INVALID_HANDLE_VALUE);
        assert(dst);

        ec.clear();
        if (bytes == 0)
            [[unlikely]] return 0;

        DWORD read_bytes;
        if (::ReadFile(src, dst, bytes, &read_bytes, NULL) == FALSE)
            [[unlikely]] ec.assign(::GetLastError(), std::system_category());

        return read_bytes;
    }

    std::size_t read(Handle src, std::byte* dst, std::size_t bytes, std::size_t offset)
    {
        assert(src != INVALID_HANDLE_VALUE);
        assert(dst);

        if (bytes == 0)
            [[unlikely]] return 0;

        using _off = decltype(OVERLAPPED::Offset);

        OVERLAPPED ov = {};
        ov.Offset     = static_cast<_off>(offset);
        ov.OffsetHigh = static_cast<_off>(offset >> (sizeof(_off) * 8));

        DWORD read_bytes;
        if (::ReadFile(src, dst, bytes, &read_bytes, &ov) == FALSE)
            [[unlikely]] throw _se(::GetLastError(), std::system_category());

        return read_bytes;
    }

    std::size_t read(Handle src, std::byte* dst, std::size_t bytes, std::size_t offset, _ec& ec) noexcept
    {
        assert(src != INVALID_HANDLE_VALUE);
        assert(dst);

        ec.clear();
        if (bytes == 0)
            [[unlikely]] return 0;

        using _off = decltype(OVERLAPPED::Offset);

        OVERLAPPED ov = {};
        ov.Offset     = static_cast<_off>(offset);
        ov.OffsetHigh = static_cast<_off>(offset >> (sizeof(_off) * 8));

        DWORD read_bytes;
        if (::ReadFile(src, dst, bytes, &read_bytes, &ov) == FALSE)
            [[unlikely]] ec.assign(::GetLastError(), std::system_category());

        return read_bytes;
    }

    std::size_t write(const std::byte* src, Handle dst, std::size_t bytes)
    {
        assert(src);
        assert(dst != INVALID_HANDLE_VALUE);
        if (bytes == 0)
            return 0;
        DWORD written_bytes;
        if (::WriteFile(dst, src, bytes, &written_bytes, NULL) == FALSE)
            [[unlikely]] throw _se(::GetLastError(), std::system_category());
        return written_bytes;
    }

    std::size_t write(const std::byte* src, Handle dst, std::size_t bytes, _ec& ec) noexcept
    {
        assert(src);
        assert(dst != INVALID_HANDLE_VALUE);
        ec.clear();
        if (bytes == 0)
            return 0;
        DWORD written_bytes;
        if (::WriteFile(dst, src, bytes, &written_bytes, NULL) == FALSE)
            [[unlikely]] ec.assign(::GetLastError(), std::system_category());
        return written_bytes;
    }

    void flush(Handle h)
    {
        assert(h != INVALID_HANDLE_VALUE);
        ::FlushFileBuffers(h);
    }

    void close(Handle h)
    {
        if (::CloseHandle(h) == 0)
            [[unlikely]] throw _se(::GetLastError(), std::system_category());
    }

} // namespace drako::io