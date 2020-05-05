#pragma once
#include "drako/system/native_io_api.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/system/native_io_file.hpp"

#include <system_error>
#include <tuple>
#include <type_traits>


// #include <WinBase.h>
// #include <errhandlingapi.h> // ::GetLastError()
// #include <fileapi.h>
#include <Windows.h>

#if !defined(DRAKO_PLT_WIN32)
#error Wrong target patform - this source file should be included only in Windows builds
#endif

namespace drako::sys
{
    [[nodiscard]] std::error_code
    read(native_file& src, std::byte* dst, size_t bytes) noexcept
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
    read_scatter(native_file& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept
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
    async_read(aio_control_block& cb, native_file& src, std::byte* dst, size_t bytes, size_t offset) noexcept
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
    async_read_scatter(native_file& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept
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

} // namespace drako::sys