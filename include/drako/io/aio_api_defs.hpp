#pragma once
#ifndef DRAKO_AIO_API_DEFS_HPP
#define DRAKO_AIO_API_DEFS_HPP

#include "drako/io/vio_api_defs.hpp"

#include <filesystem>
#include <future>
#include <tuple>

namespace drako::io::async
{
    namespace vio = drako::io::vectored;

    struct aio_result
    {
        std::size_t bytes;
    };


    // Reads data from file to memory buffer.
    [[nodiscard]] std::error_code
    read(file_handle& src, std::byte* dst, std::size_t bytes, std::size_t offset) noexcept;

    // Reads data from file to multiple memory buffers.
    [[nodiscard]] std::error_code
    read_scatter(file_handle& src, const vio::scatter_list& dst, std::size_t bytes, std::size_t offset) noexcept;

    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_read(vio::control_block& cb, file_handle& src, std::byte* dst, std::size_t bytes, std::size_t offset) noexcept;

    // Asynchronously reads data from file to multiple memory buffers.
    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_read_scatter(file_handle& src, const vio::scatter_list& dst, std::size_t bytes, std::size_t offset) noexcept;

    // Writes data from memory buffer to file.
    [[nodiscard]] std::error_code
    write(std::byte* src, file_handle& dst, std::size_t bytes, std::size_t offset) noexcept;

    // Writes data from multiple memory buffers to file.
    [[nodiscard]] std::error_code
    write_gather(const vio::gather_list& src, file_handle& dst, std::size_t bytes, std::size_t offset) noexcept;

    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_write(std::byte* src, file_handle& dst, std::size_t bytes, std::size_t offset) noexcept;

    // Asynchronously writes data from multiple memory buffers to file.
    [[nodiscard]] std::tuple<std::error_code, std::future<void>>
    async_write_gather(vio::control_block& cb, const vio::gather_list& src, std::size_t bytes, std::size_t offset) noexcept;

    // blocks until async operation completes
    [[nodiscard]] std::tuple<std::error_code, aio_result> wait(vio::control_block& cb) noexcept;

    // checks if async operation has completed
    [[nodiscard]] std::tuple<std::error_code, aio_result> try_wait(vio::control_block& cb) noexcept;

    // blocks until async operation completes or timeout ends
    [[nodiscard]] std::tuple<std::error_code, aio_result> wait_for(vio::control_block& cb) noexcept;

} // namespace drako::io::async

#endif // !DRAKO_AIO_API_DEFS_HPP