#pragma once
#ifndef DRAKO_NATIVE_IO_API_HPP
#define DRAKO_NATIVE_IO_API_HPP

#include <future>
#include <system_error>
#include <tuple>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/system/native_io_file.hpp"

namespace drako::sys
{
    struct vio_buffer_requirements
    {
        std::size_t size;
        std::size_t alignment;
    };

    DRAKO_NODISCARD vio_buffer_requirements native_vio_requirements() noexcept;


    // Reads data from file to memory buffer.
    DRAKO_NODISCARD std::error_code
    read(native_file& src, void* dst, size_t bytes, size_t offset) noexcept;

    // Reads data from file to multiple memory buffers.
    DRAKO_NODISCARD std::error_code
    read_scatter(native_file& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept;

    // Asynchronously reads data from file to multiple memory buffers.
    DRAKO_NODISCARD std::tuple<std::error_code, std::future<void>>
    read_scatter_async(native_file& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept;

    // Writes data from memory buffer to file.
    DRAKO_NODISCARD std::error_code
    write(void* src, native_file& dst, size_t bytes, size_t offset) noexcept;

    // Writes data from multiple memory buffers to file.
    DRAKO_NODISCARD std::error_code
    write_gather(const gather_list& src, native_file& dst, size_t bytes, size_t offset) noexcept;

    // Asynchronously writes data from multiple memory buffers to file.
    DRAKO_NODISCARD std::tuple<std::error_code, std::future<void>>
    write_gather_async(const gather_list& src, native_file& dst, size_t bytes, size_t offset) noexcept;

} // namespace drako::sys

#endif // !DRAKO_NATIVE_IO_API_HPP