#pragma once
#ifndef SYSTEM_DYNAMIC_LIBRARY_HPP
#define SYSTEM_DYNAMIC_LIBRARY_HPP

#if defined(_WIN32)
#include <errhandlingapi.h>
#include <libloaderapi.h>
#endif

#include <cassert>
#include <filesystem>
#include <system_error>

namespace sys
{
#if defined(_WIN32)
    using DynamicLibraryHandle = HMODULE;
#else
#error Platform not supported.
#endif

    /// @brief Load a dynamic library.
    ///
    /// @param[in] p Path of the library file.
    ///
    /// @return Handle to the library object.
    ///
    [[nodiscard]] DynamicLibraryHandle acquire(
        const std::filesystem::path& p)
    {
#if defined(_WIN32)
        const auto handle = ::LoadLibraryW(p.native());
        if (NULL == handle) [[unlikely]]
            throw std::system_error(::GetLastError(), std::system_category());
        return handle;
#endif
    }


    /// @brief Load a dynamic library.
    ///
    /// @param[in] p   Path of the library file.
    /// @param[out] ec Out-parameter for error reporting.
    ///
    /// @return Handle to the library object.
    /// 
    /// @note If the function fails, the value
    /// of the retured handle is unspecified.
    ///
    [[nodiscard]] DynamicLibraryHandle acquire(
        const std::filesystem::path& p, std::error_code& ec) noexcept
    {
#if defined(_WIN32)
        const auto handle = ::LoadLibraryW(p.native());
        if (NULL == handle) [[unlikely]]
            ec.assign(::GetLastError(), std::system_category());
        else
            ec.clear();
        return handle;
#endif
    }


    /// @brief Unload a dynamic library.
    ///
    /// @param[in] h Open handle to the library object.
    ///
    void release(const DynamicLibraryHandle& h)
    {
#if defined(_WIN32)
        assert(NULL != h);
        if (::FreeLibrary(h) == 0) [[unlikely]]
            throw std::system_error(::GetLastError(), std::system_category());
#endif
    }

    /// @brief Unload a dynamic library.
    /// 
    /// @param h Open handle to the library object.
    /// @param ec Out-parameter for error reporting.
    /// 
    void release(const DynamicLibraryHandle& h, std::error_code& ec) noexcept;


    /// @brief Load a symbol from a loaded library.
    ///
    /// @param[in] h      Handle of the library.
    /// @param[in] symbol Mangled name of the symbol.
    ///
    /// @return Pointer to the symbol.
    ///
    [[nodiscard]] void* fetch_symbol(
        const DynamicLibraryHandle h, const std::string& symbol)
    {
#if defined(_WIN32)
        assert(NULL != h);

        const auto proc = ::GetProcAddress(h, symbol.c_str());
        if (NULL == proc) [[unlikely]]
            throw std::system_error(::GetLastError(), std::system_category());
        return proc;
#endif
    }


    /// @brief Load a symbol from a loaded library.
    ///
    /// @param[in] h      Handle
    /// @param[in] symbol Mangled name of the symbol.
    /// @param[out] ec    Error code produced by the OS if any.
    ///
    /// @return Pointer to the symbol.
    ///
    [[nodiscard]] void* fetch_symbol(
        const DynamicLibraryHandle h, const std::string& symbol, std::error_code& ec) noexcept;


    /// @brief RAII wrapper for library objects.
    class DynamicLibrary
    {
    public:
        DynamicLibrary(const std::filesystem::path& p)
            : _handle{ acquire(p) }
        {
        }

        ~DynamicLibrary()
        {
            release(_handle);
        }

        [[nodiscard]] constexpr DynamicLibraryHandle native_handle() noexcept
        {
            return _handle;
        }

    private:
        DynamicLibraryHandle _handle;
    }

} // namespace sys

#endif // !SYSTEM_DYNAMIC_LIBRARY_HPP