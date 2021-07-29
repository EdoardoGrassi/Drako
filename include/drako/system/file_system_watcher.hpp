#pragma once
#ifndef SYSTEM_FILE_SYSTEM_WATCHER_HPP
#define SYSTEM_FILE_SYSTEM_WATCHER_HPP

#if defined(_WIN32)

#include "drako/concurrency/lockfree_ringbuffer.hpp"

#include <Windows.h>
#endif

#include <filesystem>
#include <thread>
#include <vector>
#include <system_error>

namespace sys
{
    /// @brief Monitor filesystem changes inside a directory subtree.
    class Watcher
    {
    public:
        /* struct Event
        {
            std::filesystem::path file;
        }; */

        explicit Watcher(const std::filesystem::path& dir);
        ~Watcher();

        Watcher(const Watcher&) = delete;
        Watcher& operator=(const Watcher&) = delete;

        struct PollResult
        {
            /// @brief Files that have been created.
            std::vector<std::filesystem::path> created;

            /// @brief Files that have been removed.
            std::vector<std::filesystem::path> removed;

            /// @brief Files that have been modified.
            std::vector<std::filesystem::path> modified;

            /// @brief Errors occurred while monitoring the filesystem.
            std::vector<std::error_code> errors;
        };

        /// @brief Retrieve recorded events.
        [[nodiscard]] PollResult poll();

        /// @brief Discard all recorded events.
        void discard();

    private:
#if defined(_WIN32)
        template <typename T>
        using _sync_queue = drako::lockfree::RingBuffer<T>;

        _sync_queue<std::filesystem::path> _created;
        _sync_queue<std::filesystem::path> _removed;
        _sync_queue<std::filesystem::path> _modified;
        _sync_queue<std::error_code>       _errors;

        std::thread      _worker; // keeps polling with ReadDirectoryChanges
        std::atomic_flag _cancel; // terminate worker execution
        HANDLE           _watcher;
        HANDLE           _read_wait;

        void _poll();
#endif
    };

} // namespace sys

#endif // !SYSTEM_FILE_SYSTEM_WATCHER_HPP