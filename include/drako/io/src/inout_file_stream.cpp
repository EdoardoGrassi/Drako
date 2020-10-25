#include "drako/io/inout_file_stream.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#if defined(_drako_platform_Win32)
#include <Windows.h>
#endif

#include <filesystem>
#include <system_error>


namespace drako::io
{
    constexpr inout_file_stream::inout_file_stream() noexcept
        : _handle{ INVALID_HANDLE_VALUE }
    {
    }

    constexpr inout_file_stream::inout_file_stream(native_handle_type handle) noexcept
        : _handle{ handle }
    {
    }

    inout_file_stream::inout_file_stream(const path_type& file, creation c = creation::if_needed)
    {
        DWORD open_mode = OPEN_ALWAYS;
        if (c == creation::open_existing)
            open_mode = OPEN_EXISTING;
        if (c == creation::truncate_existing)
            open_mode = TRUNCATE_EXISTING;

        _handle = ::CreateFileW(file.wstring().data(),
            GENERIC_READ | GENERIC_WRITE, 0, NULL, open_mode, FILE_ATTRIBUTE_NORMAL, NULL);
        if (_handle == INVALID_HANDLE_VALUE)
            throw std::system_error(::GetLastError(), std::system_category());
    }

    inout_file_stream::inout_file_stream(inout_file_stream&& other) noexcept
        : _handle{ other._handle }
    {
        other._handle = INVALID_HANDLE_VALUE;
    }

    inout_file_stream& inout_file_stream::operator=(inout_file_stream&& other) noexcept
    {
        using std::swap;
        swap(_handle, other._handle);
        return *this;
    }

    inout_file_stream::~inout_file_stream() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
            if (::CloseHandle(_handle) == 0)
                [[unlikely]] std::terminate(); // can't throw in destructor
    }

    void inout_file_stream::close()
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            if (::CloseHandle(_handle) == 0)
                [[unlikely]] throw std::system_error(::GetLastError(), std::system_category());
            _handle = INVALID_HANDLE_VALUE;
        }
    }


    //void inout_file_stream::read(std::byte* dst, size_t bytes) noexcept;

    /*
    [[nodiscard]] std::error_code
    inout_file_stream::read_scatter(const scatter_list& dst, size_t bytes, size_t offset) noexcept
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
    }*/

} // namespace drako::io