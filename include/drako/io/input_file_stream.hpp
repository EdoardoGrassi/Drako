#pragma once
#ifndef DRAKO_INPUT_FILE_STREAM_HPP
#define DRAKO_INPUT_FILE_STREAM_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/io/io_api_defs.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <filesystem>
#include <new> // placement operator new
#include <span>
#include <type_traits>

namespace drako::io
{


    /// @brief Unformatted binary input from a file.
    class input_file_stream final
    {
    public:
        using path_type = std::filesystem::path;
#if defined(DRAKO_PLT_WIN32)
        using native_handle_type = HANDLE; // win32 file handle
#else
#error Not supported on current platform.
#endif
        explicit constexpr input_file_stream() noexcept;
        explicit constexpr input_file_stream(native_handle_type) noexcept;
        explicit input_file_stream(const path_type& file);

        input_file_stream(const input_file_stream&) = delete;
        input_file_stream& operator=(const input_file_stream&) = delete;

        input_file_stream(input_file_stream&&) noexcept;
        input_file_stream& operator=(input_file_stream&&) noexcept;

        ~input_file_stream() noexcept;

        /// @brief Reads raw byte from the file.
        /// @param dst Destination buffer.
        /// @param bytes Number of bytes to read.
        void read(std::byte* dst, size_t bytes);

        void read(std::span<std::byte> buffer);

        /// @brief Close the handle.
        void close();

        [[nodiscard]] native_handle_type native_handle() noexcept;

    private:
        native_handle_type _handle;
    };


    /// @brief Instantiate an object from raw bytes.
    ///
    /// @tparam T
    /// @param input   Source input stream.
    /// @param storage Memory destination.
    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T>
    void read_from_bytes(input_file_stream& input, T& storage) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(!std::is_array_v<T>);

        const auto location = std::addressof(storage);
        input.read(reinterpret_cast<std::byte*>(location), sizeof(T));
        ::new (location) T; // placement new should result in no-op
    }


    template <typename T, size_t Size> /* clang-format off */
    requires std::is_trivially_copyable_v<T> && std::is_bounded_array_v<T>
    void read_from_bytes(input_file_stream& input, T (&storage)[Size]) /* clang-format off */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(std::is_bounded_array_v<T>);

        input.read(reinterpret_cast<std::byte*>(storage), sizeof(T) * Size);
        for (auto i = 0; i < Size; ++i)
            ::new (storage + i) T; // placement new should result in no-op
    }


    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T> //&& std::is_unbounded_array_v<T>
    void read_from_bytes(input_file_stream& input, T storage[], std::size_t size) /* clang-format off */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        //static_assert(std::is_unbounded_array_v<T>);

        input.read(reinterpret_cast<std::byte*>(storage), sizeof(T) * size);
        for (auto i = 0; i < size; ++i)
            ::new (storage + i) T; // placement new should result in no-op
    }

} // namespace drako::io

#endif // !DRAKO_INPUT_FILE_STREAM_HPP