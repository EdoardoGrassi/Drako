#pragma once
#ifndef DRAKO_OUTPUT_FILE_HANDLE_HPP
#define DRAKO_OUTPUT_FILE_HANDLE_HPP

#include "drako/core/platform.hpp"
#include "drako/io/core.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <cassert>
#include <filesystem>
#include <type_traits>

namespace drako::io
{
    /// @brief Smart handle that manages an output file.
    class UniqueOutputFile
    {
    public:
        using path_type = std::filesystem::path;

        explicit constexpr UniqueOutputFile() noexcept = default;

        explicit constexpr UniqueOutputFile(native_handle_type handle) noexcept
            : _handle{ handle } {}

        explicit UniqueOutputFile(const path_type& filename, Create c = Create::if_needed)
            : _handle{ io::open(filename, Mode::write, c) } {}

        ~UniqueOutputFile() noexcept
        {
            if (_handle != INVALID_HANDLE_VALUE)
                io::close(_handle);
        }


        UniqueOutputFile(const UniqueOutputFile&) = delete;
        UniqueOutputFile& operator=(const UniqueOutputFile&) = delete;


        UniqueOutputFile(UniqueOutputFile&& other) noexcept
            : _handle{ INVALID_HANDLE_VALUE }
        {
            using std::swap;
            swap(_handle, other._handle);
        }
        UniqueOutputFile& operator=(UniqueOutputFile&& other) noexcept
        {
            using std::swap;
            swap(_handle, other._handle);
            return *this;
        }

        [[nodiscard]] auto write(std::span<const std::byte> buf)
        {
            return io::write(std::data(buf), _handle, std::size(buf));
        }

        [[nodiscard]] auto write(std::span<const std::byte> buf, std::error_code& ec) noexcept
        {
            return io::write(std::data(buf), _handle, std::size(buf), ec);
        }

        void close()
        {
            assert(_handle != INVALID_HANDLE_VALUE);
            io::close(_handle);
        }

        void flush()
        {
            io::flush(_handle);
        }

        [[nodiscard]] native_handle_type native_handle() noexcept { return _handle; }

    private:
        native_handle_type _handle = INVALID_HANDLE_VALUE;
    };


    /// @brief Write an entire buffer.
    inline void write_all(UniqueOutputFile& f, std::span<const std::byte> buf)
    {
        auto done = 0;
        while (done < std::size(buf))
            done += f.write(buf.last(std::size(buf) - done));
    }

    /// @brief Write an entire buffer.
    [[nodiscard]] inline std::size_t write_all(
        UniqueOutputFile& f, std::span<const std::byte> buf, std::error_code& ec) noexcept;


    /// @brief Write the value representation of an object.
    /// @tparam T
    /// @param object
    /// @param ofs
    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T>
    void write_to_bytes(UniqueOutputFile& output, const T& object) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(!std::is_array_v<T>);

        auto location = std::addressof(object);
        output.write(reinterpret_cast<const std::byte*>(location), sizeof(T));
    }


    template <typename T, size_t Size> /* clang-format off */
    requires std::is_trivially_copyable_v<T> && std::is_bounded_array_v<T>
    void write_to_bytes(UniqueOutputFile& output, const T (&objects)[Size] ) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(std::is_bounded_array_v<T>);

        output.write(reinterpret_cast<const std::byte*>(objects), sizeof(T) * Size);
    }


    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T> //&& std::is_unbounded_array_v<T>
    void write_to_bytes(UniqueOutputFile& output, const T objects[], std::size_t size) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");

        output.write(reinterpret_cast<const std::byte*>(objects), sizeof(T) * size);
    }

} // namespace drako::io

#endif // !DRAKO_OUTPUT_FILE_HANDLE_HPP