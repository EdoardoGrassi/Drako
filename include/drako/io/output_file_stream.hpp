#pragma once
#ifndef DRAKO_OUTPUT_FILE_STREAM_HPP
#define DRAKO_OUTPUT_FILE_STREAM_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/io/io_api_defs.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <filesystem>
#include <type_traits>

namespace drako::io
{
    /// @brief Unformatted binary output on a file.
    class output_file_stream final
    {
    public:
        using path_type = std::filesystem::path;
#if defined(DRAKO_PLT_WIN32)
        using native_handle_type = HANDLE; // win32 file handle
#else
#error Not supported on current platform.
#endif
        constexpr explicit output_file_stream() noexcept;
        constexpr explicit output_file_stream(native_handle_type handle) noexcept;
        explicit output_file_stream(const path_type& file, creation c);
        ~output_file_stream() noexcept;

        output_file_stream(const output_file_stream&) = delete;
        output_file_stream& operator=(const output_file_stream&) = delete;

        output_file_stream(output_file_stream&&) noexcept;
        output_file_stream& operator=(output_file_stream&&) noexcept;

        void write(const std::byte* src, size_t bytes);

        void close();

        [[nodiscard]] native_handle_type native_handle() const noexcept;

    private:
        native_handle_type _handle;
    };


    /// @brief Write the value representation of an object.
    /// @tparam T 
    /// @param object 
    /// @param ofs 
    template <typename T>
    void write_to_bytes(const T& object, output_file_stream& ofs)
    {
        static_assert(std::is_trivially_copyable_v<T>, "Required by C++ standard");
        static_assert(!std::is_array_v<T>);
        ofs.write(reinterpret_cast<const std::byte*>(object), sizeof(T));
    }

} // namespace drako::io

#endif // !DRAKO_OUTPUT_FILE_STREAM_HPP