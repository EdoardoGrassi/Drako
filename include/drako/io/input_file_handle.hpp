#pragma once
#ifndef DRAKO_INPUT_FILE_HANDLE_HPP
#define DRAKO_INPUT_FILE_HANDLE_HPP

#include "drako/core/platform.hpp"
#include "drako/io/core.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <cassert>
#include <filesystem>
#include <span>
#include <type_traits>

namespace drako::io
{
    /// @brief Smart handle that manages an input file.
    class UniqueInputFile
    {
    public:
        using path_type = std::filesystem::path;

        explicit constexpr UniqueInputFile() noexcept;

        explicit constexpr UniqueInputFile(native_handle_type handle) noexcept
            : _handle{ handle } {}

        explicit UniqueInputFile(const path_type& filename)
            : _handle{ io::open(filename, mode::read, creation::open_existing) } {}


        UniqueInputFile(const UniqueInputFile&) = delete;
        UniqueInputFile& operator=(const UniqueInputFile&) = delete;


        UniqueInputFile(UniqueInputFile&& other) noexcept
        {
            using std::swap;
            swap(_handle, other._handle);
        }
        UniqueInputFile& operator=(UniqueInputFile&& other) noexcept
        {
            using std::swap;
            swap(_handle, other._handle);
            return *this;
        }

        ~UniqueInputFile()
        {
            if (_handle != INVALID_HANDLE_VALUE)
                io::close(_handle);
        }

        /// @brief Reads raw bytes from the file.
        /// @param dst Destination buffer.
        /// @param bytes Number of bytes to read.
        void read(std::byte* dst, std::size_t bytes)
        {
            io::read(_handle, dst, bytes);
        }
        void read(std::byte* dst, std::size_t bytes, std::size_t offset)
        {
            io::read(_handle, dst, bytes, offset);
        }

        /// @brief Reads raw bytes from the file.
        void read(std::byte* dst, std::size_t bytes, std::error_code& ec) noexcept
        {
            io::read(_handle, dst, bytes, ec);
        }
        void read(std::byte* dst, std::size_t bytes, std::size_t offset, std::error_code& ec) noexcept
        {
            io::read(_handle, dst, bytes, offset, ec);
        }

        /// @brief Close the handle.
        void close()
        {
            assert(_handle != INVALID_HANDLE_VALUE);
            io::close(_handle);
        }

        [[nodiscard]] native_handle_type native_handle() noexcept { return _handle; }

    private:
        native_handle_type _handle = INVALID_HANDLE_VALUE;
    };

    inline void read(
        UniqueInputFile input, std::span<std::byte> buffer)
    {
        input.read(std::data(buffer), std::size(buffer));
    }

    inline void read(
        UniqueInputFile input, std::span<std::byte> buffer, std::error_code& ec) noexcept
    {
        input.read(std::data(buffer), std::size(buffer), ec);
    }


    /// @brief Instantiate an object from raw bytes.
    ///
    /// @tparam T
    /// @param input   Source input stream.
    /// @param storage Memory destination.
    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T>
    inline void read_from_bytes(UniqueInputFile& input, T& storage) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(!std::is_array_v<T>);

        const auto location = std::addressof(storage);
        input.read(reinterpret_cast<std::byte*>(location), sizeof(T));
    }


    template <typename T, size_t Size> /* clang-format off */
    requires std::is_trivially_copyable_v<T> && std::is_bounded_array_v<T>
    inline void read_from_bytes(UniqueInputFile& input, T (&storage)[Size]) /* clang-format off */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(std::is_bounded_array_v<T>);

        input.read(reinterpret_cast<std::byte*>(storage), sizeof(T) * Size);
    }


    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T> //&& std::is_unbounded_array_v<T>
    inline void read_from_bytes(UniqueInputFile& input, T storage[], std::size_t size) /* clang-format off */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        //static_assert(std::is_unbounded_array_v<T>);

        input.read(reinterpret_cast<std::byte*>(storage), sizeof(T) * size);
    }

} // namespace drako::io

#endif // !DRAKO_INPUT_FILE_HANDLE_HPP