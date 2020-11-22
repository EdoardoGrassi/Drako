#pragma once
#ifndef DRAKO_OUTPUT_FILE_HANDLE_HPP
#define DRAKO_OUTPUT_FILE_HANDLE_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/io/core.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#endif

#include <filesystem>
#include <type_traits>

namespace drako::io
{
    /// @brief Unformatted binary output on a file.
    class output_file_handle final
    {
    public:
        using path_type = std::filesystem::path;

        explicit constexpr output_file_handle() noexcept = default;

        explicit constexpr output_file_handle(native_handle_type handle) noexcept
            : _handle{ handle } {}

        explicit output_file_handle(const path_type& filename, creation c = creation::if_needed)
            : _handle{ io::open(filename, mode::write, c) } {}

        ~output_file_handle() noexcept;


        output_file_handle(const output_file_handle&) = delete;
        output_file_handle& operator=(const output_file_handle&) = delete;


        output_file_handle(output_file_handle&& other) noexcept
            : _handle{ INVALID_HANDLE_VALUE }
        {
            using std::swap;
            swap(_handle, other._handle);
        }
        output_file_handle& operator=(output_file_handle&& other) noexcept
        {
            using std::swap;
            swap(_handle, other._handle);
            return *this;
        }

        void write(const std::byte* src, std::size_t bytes);

        void write(const std::byte* src, std::size_t bytes, std::error_code& ec) noexcept;

        void close();

        void flush();

        [[nodiscard]] native_handle_type native_handle() noexcept { return _handle; }

    private:
        native_handle_type _handle = INVALID_HANDLE_VALUE;
    };

    using OutputFileHandle = output_file_handle;


    /// @brief Write the value representation of an object.
    /// @tparam T
    /// @param object
    /// @param ofs
    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T>
    void write_to_bytes(output_file_handle& output, const T& object) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(!std::is_array_v<T>);

        auto location = std::addressof(object);
        output.write(reinterpret_cast<const std::byte*>(location), sizeof(T));
    }


    template <typename T, size_t Size> /* clang-format off */
    requires std::is_trivially_copyable_v<T> && std::is_bounded_array_v<T>
    void write_to_bytes(output_file_handle& output, const T (&objects)[Size] ) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");
        static_assert(std::is_bounded_array_v<T>);

        output.write(reinterpret_cast<const std::byte*>(objects), sizeof(T) * Size);
    }


    template <typename T> /* clang-format off */
    requires std::is_trivially_copyable_v<T> //&& std::is_unbounded_array_v<T>
    void write_to_bytes(output_file_handle& output, const T objects[], std::size_t size) /* clang-format on */
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "Required by C++ standard for well-defined type punning.");

        output.write(reinterpret_cast<const std::byte*>(objects), sizeof(T) * size);
    }

} // namespace drako::io

#endif // !DRAKO_OUTPUT_FILE_HANDLE_HPP