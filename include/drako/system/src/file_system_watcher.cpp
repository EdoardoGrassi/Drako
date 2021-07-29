#include "drako/system/file_system_watcher.hpp"

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <array>
#include <cassert>
#include <filesystem>
#include <system_error>
#include <thread>

#include <iostream>

namespace sys
{
    namespace fs = std::filesystem;

#if defined(_WIN32)

    // extract the path of the filesystem object involved in an event
    [[nodiscard]] fs::path _parse_path_win32(const FILE_NOTIFY_INFORMATION* fni)
    {
        return fs::path{ std::wstring{ fni->FileName,
            fni->FileNameLength / sizeof(fni->FileName[0]) } };
    }

    [[nodiscard]] void _parse_report_win32(const std::byte* bytes)
    {
    }

    void Watcher::_poll()
    {
        const auto filter = FILE_NOTIFY_CHANGE_FILE_NAME
                          | FILE_NOTIFY_CHANGE_CREATION
                          | FILE_NOTIFY_CHANGE_SIZE
                          | FILE_NOTIFY_CHANGE_LAST_WRITE;

        while (_cancel.test()) //test_and_set()
        {
            // the report buffer must be DWORD-aligned
            alignas(alignof(DWORD)) std::array<std::byte, 2048> buffer;
            static_assert(sizeof(buffer) > sizeof(FILE_NOTIFY_INFORMATION),
                "required buffer capacity to read at least one notification");

            DWORD bytes = 0;
            //OVERLAPPED overlapped{};
            //overlapped.hEvent = _read_wait;

            const auto ok = ::ReadDirectoryChangesW(_watcher,
                std::data(buffer), std::size(buffer),
                TRUE, // watch subtree recursively
                filter, &bytes,
                //&overlapped,
                NULL, NULL);

            if (!ok)
            {
                if (ok == ERROR_OPERATION_ABORTED)
                    return; // end worker task
                else
                    _errors.enque(std::error_code(::GetLastError(), std::system_category()));
                //std::cout << std::error_code(::GetLastError(), std::system_category()).message() << '\n';
                //throw std::system_error(::GetLastError(), std::system_category());
            }

            /*
            switch (const auto state = ::WaitForSingleObject(_read_wait, INFINITE); state)
            {
                case WAIT_OBJECT_0:
                {
                    if (bytes != 0)
                    {
                        for (auto next_info_entry = std::data(buffer);;)
                        {
                            FILE_NOTIFY_INFORMATION info;
                            std::memcpy(&info, next_info_entry, sizeof(info));

                            switch (info.Action)
                            {
                                case FILE_ACTION_ADDED:
                                    _created.push(_file_path_win32(info));
                                    break;

                                case FILE_ACTION_REMOVED:
                                    _removed.push(_file_path_win32(info));
                                    break;

                                case FILE_ACTION_MODIFIED:
                                    _modified.push(_file_path_win32(info));
                                    break;

                                default: break; // discard other kinds of events
                            }

                            if (info.NextEntryOffset == 0)
                                break;

                            next_info_entry += info.NextEntryOffset;
                        }
                    }
                    break;
                }

                case WAIT_FAILED:
                    throw std::system_error(::GetLastError(), std::system_category());
            }
            */
            if (bytes != 0)
            {
                for (auto next_info_entry = std::data(buffer);;)
                {
                    //FILE_NOTIFY_INFORMATION info;
                    //std::memcpy(&info, next_info_entry, sizeof(info));
                    const auto info = new (next_info_entry) FILE_NOTIFY_INFORMATION;
                    switch (info->Action)
                    {
                        case FILE_ACTION_ADDED:
                            std::wcout << _parse_path_win32(info);
                            _created.enque(_parse_path_win32(info));
                            break;

                        case FILE_ACTION_REMOVED:
                            std::wcout << _parse_path_win32(info);
                            _removed.enque(_parse_path_win32(info));
                            break;

                        case FILE_ACTION_MODIFIED:
                            std::wcout << _parse_path_win32(info);
                            _modified.enque(_parse_path_win32(info));
                            break;

                        default: break; // discard other kinds of events
                    }

                    if (info->NextEntryOffset == 0)
                        break;

                    next_info_entry += info->NextEntryOffset;
                    std::destroy_at(info);
                }
            }
        }
    }
#endif


    Watcher::Watcher(const fs::path& dir)
        : _created{ 256 }, _removed{ 256 }, _modified{ 1024 }, _errors{ 128 }
    {
        assert(!std::empty(dir));
        if (!fs::is_directory(dir))
            throw std::invalid_argument{ "expected a directory" };

#if defined(_WIN32)
        _watcher = ::CreateFileW(dir.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL);
        if (_watcher == INVALID_HANDLE_VALUE)
            throw std::system_error(::GetLastError(), std::system_category());

        _read_wait = ::CreateEventW(NULL, FALSE, FALSE, NULL);
        if (_read_wait == NULL)
            throw std::system_error(::GetLastError(), std::system_category());

        _cancel.test_and_set();
        _worker = std::thread(&Watcher::_poll, this);
#endif
    }

    Watcher::~Watcher()
    {
#if defined(_WIN32)
        assert(_read_wait != INVALID_HANDLE_VALUE);
        assert(_watcher != INVALID_HANDLE_VALUE);

        // signal the worker, then unlock it
        _cancel.clear();
        //if (::SetEvent(_read_wait) == 0) [[unlikely]]
        //    std::terminate();

        ::CancelSynchronousIo(_worker.native_handle());
        _worker.join();

        if (::CloseHandle(_watcher) == 0) [[unlikely]]
            std::terminate(); // cannot throw in destructor

        if (::CloseHandle(_read_wait) == 0) [[unlikely]]
            std::terminate(); // ^^^
#endif
    }

    Watcher::PollResult Watcher::poll()
    {
#if defined(_WIN32)
        /*
        const auto filter = FILE_NOTIFY_CHANGE_CREATION |
                            FILE_NOTIFY_CHANGE_SIZE |
                            FILE_NOTIFY_CHANGE_LAST_WRITE;

        // the report buffer must be DWORD-aligned
        alignas(alignof(DWORD)) std::array<std::byte, 2048> buffer;
        static_assert(sizeof(buffer) > sizeof(FILE_NOTIFY_INFORMATION),
            "required buffer capacity to read at least one notification");

        DWORD bytes;
        if (::ReadDirectoryChangesW(_watcher,
                std::data(buffer), std::size(buffer),
                TRUE, // watch subtree recursively
                filter,
                &bytes,
                NULL, NULL) == 0)
            throw std::system_error(::GetLastError(), std::system_category());

        PollResult result;
        if (bytes != 0)
        {
            for (auto next_info_entry = std::data(buffer);;)
            {
                FILE_NOTIFY_INFORMATION info;
                std::memcpy(&info, next_info_entry, sizeof(info));

                switch (info.Action)
                {
                    case FILE_ACTION_ADDED:
                        result.created.emplace_back(_file_path_win32(info));
                        break;

                    case FILE_ACTION_REMOVED:
                        result.removed.emplace_back(_file_path_win32(info));
                        break;

                    case FILE_ACTION_MODIFIED:
                        result.modified.emplace_back(_file_path_win32(info));
                        break;

                    default: break; // discard other kinds of events
                }

                if (info.NextEntryOffset == 0)
                    break;

                next_info_entry += info.NextEntryOffset;
            }
        }
        */

        PollResult pr;
        for (fs::path p; _created.deque(p);)
            pr.created.emplace_back(std::move(p));

        for (fs::path p; _removed.deque(p);)
            pr.removed.emplace_back(std::move(p));

        for (fs::path p; _modified.deque(p);)
            pr.modified.emplace_back(std::move(p));

        for (std::error_code ec; _errors.deque(ec);)
            pr.errors.emplace_back(ec);

        return pr;
#endif
    }

} // namespace sys