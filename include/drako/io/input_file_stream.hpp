#pragma once
#ifndef DRAKO_INPUT_FILE_STREAM_HPP
#define DRAKO_INPUT_FILE_STREAM_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/io/io_api_defs.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <filesystem>
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


    /// @brief Read the value representation of an object.
    /// @tparam T
    /// @param object
    /// @param ifs
    template <typename T>
    void read_from_bytes(T& object, input_file_stream& ifs) requires std::is_trivially_copyable_v<T>
    {
        static_assert(std::is_trivially_copyable_v<T>, "Required by C++ standard");
        static_assert(!std::is_array_v<T>);
        ifs.read(reinterpret_cast<std::byte*>(object), sizeof(T));
    }

    template <typename T>
    void read_from_bytes(T object[], size_t size, input_file_stream& ifs) requires std::is_trivially_copyable_v<T>
    {
        static_assert(std::is_trivially_copyable_v<T>, "Required by C++ standard");
        ifs.read(reinterpret_cast<std::byte*>(object), sizeof(T) * size);
    }

    //template <typename T>
    //void read_from_bytes(std::span<T> object, input_file_stream& ifs) requires std::is_trivially_copyable_v<T>;

} // namespace drako::io

#endif // !DRAKO_INPUT_FILE_STREAM_HPP