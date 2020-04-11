#include "drako/system/native_io_file.hpp"

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include <future>
#include <system_error>
#include <tuple>
#include <vector>

// #include <errhandlingapi.h> // ::GetLastError()
// #include <fileapi.h>
// #include <handleapi.h>
// #include <synchapi.h> // ::WaitForSingleObject()
// #include <winerror.h>
#include <Windows.h>

#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

namespace drako::sys
{
    DRAKO_FORCE_INLINE
    constexpr native_file::native_file() noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
    }

    DRAKO_FORCE_INLINE
    constexpr native_file::native_file(native_file&& other) noexcept
        : _handle{ other._handle }
    {
        other._handle = INVALID_HANDLE_VALUE;
    }

    constexpr native_file&
    native_file::operator=(native_file&& other) noexcept
    {
        if (this != other)
        {
            _handle       = other._handle;
            other._handle = INVALID_HANDLE_VALUE;
        }
        return *this;
    }

    native_file::~native_file() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            if (::CloseHandle(_handle) == 0)
            {
                const std::error_code ec(::GetLastError(), std::system_category());
                DRAKO_LOG_ERROR("[Win32] Failed call to CloseHandle : " + ec.message());
                std::exit(EXIT_FAILURE);
            }
        }
    }

    DRAKO_NODISCARD std::error_code
    native_file::open(const path_type& p, access_mode a, share_mode s) noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
            return std::make_error_code(std::errc::bad_file_descriptor);

        _handle = ::CreateFileW(p.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (_handle == INVALID_HANDLE_VALUE)
        {
            return std::error_code(::GetLastError(), std::system_category());
        }
        return std::error_code{};
    }

    DRAKO_NODISCARD std::error_code
    native_file::close() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            if (::CloseHandle(_handle) == 0)
                return std::error_code(::GetLastError(), std::system_category());

            _handle = INVALID_HANDLE_VALUE;
        }
        return std::error_code{};
    }


    DRAKO_NODISCARD std::error_code
    native_file::read(void* dst, size_t bytes, size_t offset) noexcept
    {
        if (_handle == INVALID_HANDLE_VALUE)
            return std::make_error_code(std::errc::bad_file_descriptor);
        if (bytes == 0)
            return std::make_error_code(std::errc::invalid_argument);

        DWORD read_bytes;
        if (const auto result = ::ReadFile(_handle, dst, bytes, &read_bytes, nullptr);
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
    native_file::read_scatter(const scatter_list& dst, size_t bytes, size_t offset) noexcept
    {
        if (_handle == INVALID_HANDLE_VALUE)
            return std::make_error_code(std::errc::bad_file_descriptor);
        if (bytes == 0)
            return std::make_error_code(std::errc::invalid_argument);

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

        if (const auto result = ::ReadFileScatter(_handle, segments.data(), bytes, NULL, &options);
            result != 0)
        {
            return std::error_code{};
        }
        else
        {
            switch (const auto err = ::GetLastError(); err)
            {
                case ERROR_IO_PENDING: // TODO: wait for async termination
                    ::WaitForSingleObject(_handle, 0);
                default: return std::error_code(err, std::system_category());
            }
        }
    }

} // namespace drako::sys