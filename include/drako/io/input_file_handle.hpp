#pragma once
#ifndef DRAKO_INPUT_FILE_HANDLE_HPP
#define DRAKO_INPUT_FILE_HANDLE_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
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
    class input_file_handle
    {
    public:
        using path_type = std::filesystem::path;

        explicit constexpr input_file_handle() noexcept;

        explicit constexpr input_file_handle(native_handle_type handle) noexcept
            : _handle{ handle } {}

        explicit input_file_handle(const path_type& filename)
            : _handle{ io::open(filename, mode::read, creation::open_existing) } {}


        input_file_handle(const input_file_handle&) = delete;
        input_file_handle& operator=(const input_file_handle&) = delete;


        input_file_handle(input_file_handle&& other) noexcept
        {
            using std::swap;
            swap(_handle, other._handle);
        }
        input_file_handle& operator=(input_file_handle&& other) noexcept
        {
            using std::swap;
            swap(_handle, other._handle);
            return *this;
        }

        ~input_file_handle() noexcept
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

    using InputFileHandle = input_file_handle;


    inline void read(
        input_file_handle input, std::span<std::byte> buffer)
    {
        input.read(std::data(buffer), std::size(buffer));
    }

    inline void read(
        input_file_handle input, std::span<std::byte> buffer, std::error_code& ec) noexcept
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
    inline void read_from_bytes(
        input_file_handle& input, T& storage) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(!std::is_array_v<T>);

        const auto location = std::addressof(storage);
        input.read(reinterpret_cast<std::byte*>(location), sizeof(T));
    }


    template <typename T, size_t Size> /* clang-format off */
    requires std::is_trivially_copyable_v<T> && std::is_bounded_array_v<T>
    inline void read_from_bytes(
        input_file_handle& input, T (&storage)[Size]) /* clang-format off */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(std::is_bounded_array_v<T>);

        input.read(reinterpret_cast<std::byte*>(storage), sizeof(T) * Size);
    }


    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T> //&& std::is_unbounded_array_v<T>
    inline void read_from_bytes(
        input_file_handle& input, T storage[], std::size_t size) /* clang-format off */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        //static_assert(std::is_unbounded_array_v<T>);

        input.read(reinterpret_cast<std::byte*>(storage), sizeof(T) * size);
    }

} // namespace drako::io

#endif // !DRAKO_INPUT_FILE_HANDLE_HPP