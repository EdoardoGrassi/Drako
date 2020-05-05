#pragma once
#ifndef DRAKO_NATIVE_IO_API_HPP_
#define DRAKO_NATIVE_IO_API_HPP_

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/system/native_io_file.hpp"

#include <future>
#include <system_error>
#include <tuple>

namespace drako::sys
{
    struct vio_buffer_requirements
    {
        std::size_t size;
        std::size_t alignment;
    };

    [[nodiscard]] vio_buffer_requirements native_vio_requirements() noexcept;

    struct aio_control_block
    {
        aio_control_block(const aio_control_block&) = delete;
        aio_control_block& operator=(const aio_control_block&) = delete;

        aio_control_block(aio_control_block&&) = delete;
        aio_control_block& operator=(aio_control_block&&) = delete;

#if defined(DRAKO_PLT_WIN32)
        HANDLE     handle;
        OVERLAPPED overlapped;
#endif
    };

    struct aio_result
    {
        size_t bytes;
    };


    // Reads data from file to memory buffer.
    [[nodiscard]] std::error_code
    read(native_file& src, std::byte* dst, size_t bytes, size_t offset) noexcept;

    // Reads data from file to multiple memory buffers.
    [[nodiscard]] std::error_code
    read_scatter(native_file& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept;

    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_read(aio_control_block& cb, native_file& src, std::byte* dst, size_t bytes, size_t offset) noexcept;

    // Asynchronously reads data from file to multiple memory buffers.
    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_read_scatter(native_file& src, const scatter_list& dst, size_t bytes, size_t offset) noexcept;

    // Writes data from memory buffer to file.
    [[nodiscard]] std::error_code
    write(std::byte* src, native_file& dst, size_t bytes, size_t offset) noexcept;

    // Writes data from multiple memory buffers to file.
    [[nodiscard]] std::error_code
    write_gather(const gather_list& src, native_file& dst, size_t bytes, size_t offset) noexcept;

    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_write(std::byte* src, native_file& dst, size_t bytes, size_t offset) noexcept;

    // Asynchronously writes data from multiple memory buffers to file.
    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_write_gather(aio_control_block& cb, const gather_list& src, size_t bytes, size_t offset) noexcept;

    // blocks until async operation completes
    [[nodiscard]] std::tuple<std::error_code, aio_result> wait(aio_control_block& cb) noexcept;

    // checks if async operation has completed
    [[nodiscard]] std::tuple<std::error_code, aio_result> try_wait(aio_control_block& cb) noexcept;

    // blocks until async operation completes or timeout ends
    [[nodiscard]] std::tuple<std::error_code, aio_result> wait_for(aio_control_block& cb) noexcept;

} // namespace drako::sys

#endif // !DRAKO_NATIVE_IO_API_HPP_