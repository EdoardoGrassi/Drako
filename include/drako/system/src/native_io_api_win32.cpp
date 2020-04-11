#include "drako/system/native_io_api.hpp"

#include <system_error>
#include <type_traits>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/system/native_io_file.hpp"


// #include <WinBase.h>
// #include <errhandlingapi.h> // ::GetLastError()
// #include <fileapi.h>
#include <Windows.h>

#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

namespace drako::sys
{
    DRAKO_NODISCARD std::error_code
    read(native_file& src, void* dst, size_t bytes) noexcept
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


    DRAKO_NODISCARD std::error_code
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


    DRAKO_NODISCARD std::tuple<std::error_code, std::future<void>>
    read_scatter_async(native_file& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept
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
            result != 0)
        {
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

} // namespace drako::sys