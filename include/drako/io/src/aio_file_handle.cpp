#pragma once
#include "drako/io/aio_file_handle.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/io/aio_api_defs.hpp"

#if defined(_drako_platform_Win32)
#include <Windows.h>
#endif

#include <system_error>
#include <tuple>
#include <type_traits>


namespace drako::io::async
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
            0,
            NULL,
            NULL,
            0,
            NULL);
        if (_handle == INVALID_HANDLE_VALUE)
            throw std ::system_error(::GetLastError(), std::system_category());
    }

    file_handle::file_handle(file_handle&& other) noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
        using std::swap;
        swap(_handle, other._handle);
    }

    file_handle& file_handle::operator=(file_handle&& other) noexcept
    {
        using std::swap;
        swap(_handle, other._handle);
        return *this;
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
                throw std::system_error(::GetLastError(), std::system_category());
            _handle = INVALID_HANDLE_VALUE;
        }
    }

    /*
    [[nodiscard]] std::error_code
    read(aio_file_stream& src, std::byte* dst, size_t bytes) noexcept
    {
        if (bytes == 0)
            return std::error_code{};

        DWORD read_bytes;
        if (const auto result = ::ReadFile(src, dst, bytes, &read_bytes, nullptr);
            result == TRUE)
        {
            return std::error_code{};
        }
        else
        {
            return std::error_code(::GetLastError(), std::system_category());
        }
    }

    [[nodiscard]] std::error_code
    read_scatter(inout_file_stream& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept
    {
        if (bytes == 0)
            return std::error_code{};

        std::vector<FILE_SEGMENT_ELEMENT> segments{ std::size(dst) };
        for (const auto& buffer : dst)
        {
            FILE_SEGMENT_ELEMENT segment = {};
            segment.Buffer               = PtrToPtr64(buffer.buffer);
            segments.emplace_back(segment);
        }

        OVERLAPPED options = {};
        options.Offset     = offset;
        static_assert(std::is_unsigned_v<decltype(offset)>, "Required well defined operator>>");
        options.OffsetHigh = offset >> (sizeof(OVERLAPPED::Offset) * 8);

        if (const auto result = ::ReadFileScatter(src, segments.data(), bytes, NULL, &options);
            result != 0)
        {
            return std::error_code{};
        }
        else
        {
            switch (const auto err = ::GetLastError(); err)
            {
                case ERROR_IO_PENDING: // TODO: wait for async termination

                default:
                    return std::error_code(err, std::system_category());
            }
        }
    }

    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_read(aio_control_block& cb, inout_file_stream& src, std::byte* dst, size_t bytes, size_t offset) noexcept
    {
        if (const auto result = ::ReadFileEx(src, dst, bytes, &cb.overlapped, nullptr);
            result == 0)
        { // syscall failure
            return { std::error_code(::GetLastError(), std::system_category()), {} };
        }
        else
        {
            switch (const auto err = ::GetLastError())
            {
                case ERROR_IO_PENDING:
                    return;
            }
        }
    }

    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_read_scatter(inout_file_stream& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept
    {
        if (bytes == 0)
            return { std::error_code{}, std::future<void>{} };

        std::vector<FILE_SEGMENT_ELEMENT> segments{ std::size(dst) };
        for (const auto& buffer : dst)
        {
            FILE_SEGMENT_ELEMENT segment = {};
            segment.Buffer               = PtrToPtr64(buffer.buffer);
            segments.emplace_back(segment);
        }

        OVERLAPPED options = {};
        options.Offset     = offset;
        static_assert(std::is_unsigned_v<decltype(offset)>, "Required well defined operator>>");
        options.OffsetHigh = offset >> (sizeof(OVERLAPPED::Offset) * 8);

        if (const auto result = ::ReadFileScatter(src, segments.data(), bytes, NULL, &options);
            result == 0)
        { // syscall failure
            return { std::error_code(::GetLastError(), std::system_category()), std::future<void>{} };
        }
        else
        {
            switch (const auto err = ::GetLastError(); err)
            {
                case ERROR_IO_PENDING: // TODO: wait for async termination

                default:
                    return { std::error_code(err, std::system_category()), std::future<void>{} };
            }
        }
    }

    [[nodiscard]] std::tuple<std::error_code, aio_result>
    wait(aio_control_block& cb) noexcept
    {
        DWORD transferred_bytes;
        if (const auto syscall_result = ::GetOverlappedResult(
                cb.handle, &cb.overlapped, &transferred_bytes, TRUE);
            syscall_result == 0)
        { // syscall failure
            return { std::error_code(::GetLastError(), std::system_category()), {} };
        }
        aio_result result = {};
        result.bytes      = transferred_bytes;
        return { std::error_code{}, result };
    }

    [[nodiscard]] std::tuple<std::error_code, aio_result>
    try_wait(aio_control_block& cb) noexcept
    {
        DWORD transferred_bytes;
        if (const auto syscall_result = ::GetOverlappedResult(
                cb.handle, &cb.overlapped, &transferred_bytes, FALSE);
            syscall_result == 0)
        { // syscall failure or operation still pending
            switch (const auto err = ::GetLastError())
            {
                case ERROR_IO_PENDING:
                    return { {}, {} }; // incomplete

                default:
                    return { std::error_code(err, std::system_category()), {} };
            }
        }
        aio_result result = {};
        result.bytes      = transferred_bytes;
        return { std::error_code{}, result };
    }
    */

} // namespace drako::io::async