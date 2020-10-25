#pragma once
#ifndef DRAKO_VIO_API_DEFS_HPP
#define DRAKO_VIO_API_DEFS_HPP

#include <filesystem>

namespace drako::io::vectored
{
    struct buffer_requirements
    {
        std::size_t size;
        std::size_t alignment;
    };

    struct buffer_info
    {
        std::byte* buffer;
    };

    using scatter_list = std::vector<buffer_info>;
    using gather_list  = std::vector<buffer_info>;

    [[nodiscard]] buffer_requirements native_vio_requirements() noexcept;

    class file_handle; // forward declaration

    struct control_block
    {
        control_block(const control_block&) = delete;
        control_block& operator=(const control_block&) = delete;

        control_block(control_block&&) = delete;
        control_block& operator=(control_block&&) = delete;

#if defined(DRAKO_PLT_WIN32)
        HANDLE     handle;
        OVERLAPPED overlapped;
#endif
    };

} // namespace drako::io::vectored

#endif // !DRAKO_VIO_API_DEFS_HPP