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
            : _handle{ io::open(filename, Mode::read, Create::open_existing) } {}


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

        /// @brief Reads bytes from the file.
        /// @param buf Destination buffer.
        [[nodiscard]] auto read(
            std::span<std::byte> buf)
        {
            return io::read(_handle, std::data(buf), std::size(buf));
        }
        [[nodiscard]] auto read(
            std::span<std::byte> buf, std::size_t offset)
        {
            return io::read(_handle, std::data(buf), std::size(buf), offset);
        }

        /// @brief Reads bytes from the file.
        [[nodiscard]] auto read(
            std::span<std::byte> buf, std::error_code& ec) noexcept
        {
            return io::read(_handle, std::data(buf), std::size(buf), ec);
        }
        [[nodiscard]] auto read(
            std::span<std::byte> buf, std::size_t offset, std::error_code& ec) noexcept
        {
            return io::read(_handle, std::data(buf), std::size(buf), ec);
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
    static_assert(!std::is_copy_constructible_v<UniqueInputFile>,
        DRAKO_STRINGIZE(UniqueInputFile) " must be a movable-only type.");
    static_assert(!std::is_copy_assignable_v<UniqueInputFile>,
        DRAKO_STRINGIZE(UniqueInputFile) " must be a movable-only type.");
    static_assert(std::is_nothrow_move_constructible_v<UniqueInputFile>,
        DRAKO_STRINGIZE(UniqueInputFile) " must be nothrow movable.");
    static_assert(std::is_nothrow_move_assignable_v<UniqueInputFile>,
        DRAKO_STRINGIZE(UniqueInputFile) " must be nothrow movable.");

    /// @brief Read some bytes into a buffer.
    [[nodiscard]] inline auto read_some(
        UniqueInputFile& f, std::span<std::byte> buf)
    {
        return f.read(buf);
    }

    /// @brief Read some bytes into a buffer.
    [[nodiscard]] inline auto read_some(
        UniqueInputFile& f, std::span<std::byte> buf, std::error_code& ec) noexcept
    {
        return f.read(buf, ec);
    }


    /// @brief Read bytes into a buffer until is full.
    [[nodiscard]] inline void read_exact(
        UniqueInputFile& f, std::span<std::byte> buf)
    {
        auto done = 0; // total bytes
        auto read = 0; // last read
        do
        {
            read = f.read(buf.last(std::size(buf) - done));
            done += read;
        } while (read != 0);

        if (done != std::size(buf)) // end of file is not at the expected offset
            [[unlikely]] throw std::runtime_error{ "Unexpected end of file." };
    }

    /*
    /// @brief Read all the bytes from a file.
    [[nodiscard]] inline std::vector<std::byte> read_to_end(
        UniqueInputFile& f)
    {
        std::vector<std::byte> buffer{};
        // TODO: implementation
    }
    */


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