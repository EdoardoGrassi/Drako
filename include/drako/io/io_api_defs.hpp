#pragma once
#ifndef DRAKO_IO_API_DEFS_HPP
#define DRAKO_IO_API_DEFS_HPP

#include <cstdint>

namespace drako::io
{
    /// @brief Operations permitted for a new file handle.
    /// Combinations of different flags are accepted.
    enum class mode : std::uint8_t
    {
        // Permits read operations on the file.
        read = (1 << 0),

        // Permits write operations on the file.
        write = (1 << 1),

        // Permits read and write operations on the file
        read_write = (read | write)
    };


    // Specifies the level of access permitted for a file that has already an open handle.
    // Combinations of different flags are accepted
    // File operations that don't meet sharing restrictions will always fail.
    enum class share_mode : std::uint8_t
    {
        // Prevents sharing of the file.
        exclusive = 0,

        // Share read permission.
        read = (1 << 0),

        // Share write permission.
        write = (1 << 1),

        // Share read and write permission.
        read_write = (read | write),

        // Share delete permission.
        deletable = (1 << 2),

        // Share permission with child processes that inherits the handle.
        inheritable = (1 << 3)
    };


    enum class creation
    {
        /// @brief Open only already existing files.
        open_existing,

        /// @brief Create file if doesn't exist.
        if_needed,

        /// @brief Truncate only already existing files.
        truncate_existing
    };
} // namespace drako::io

#endif // !DRAKO_IO_API_DEFS_HPP