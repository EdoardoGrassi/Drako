#pragma once
#ifndef DRAKO_SYSTEM_MEMORY_HPP
#define DRAKO_SYSTEM_MEMORY_HPP

#include "drako/core/memory/memory_core.hpp"
#include "drako/core/compiler.hpp"
#include "drako/core/platform.hpp"
#include "drako/devel/assertion.hpp"

#if defined(DRAKO_PLT_WIN32)
#include <heapapi.h>
#include <memoryapi.h>
#endif

namespace drako::sys
{
    // Pointer to raw memory that is aligned to the OS virtual memory page size.
    // Aligned at 4Kb on both Windows and Linux.
    using mem_page_ptr = drako::aligned_ptr<4096>;


    DRAKO_NODISCARD DRAKO_ALLOCATOR void* heap_alloc(size_t bytes) noexcept
    {
        return ::HeapAlloc(::GetProcessHeap(), 0, bytes);
    }

    template <size_t Align>
    drako::aligned_ptr<Align> heap_aligned_alloc(size_t bytes) noexcept;

    void heap_dealloc(void* ptr) noexcept
    {
        ::HeapFree(::GetProcessHeap(), 0, ptr);
    }

    //
    // @brief       Reserve a range of virtual memory.
    //
    // @returns     Starting address of the range, nullptr on failure.
    //
    DRAKO_NODISCARD DRAKO_ALLOCATOR mem_page_ptr
    reserve_virtual_range(size_t bytes) noexcept
    {
        return drako::make_aligned(::VirtualAlloc(nullptr, bytes, MEM_RESERVE, PAGE_READWRITE));
    }

    //
    // @brief       Reserve a range of virtual memory.
    //
    // @returns     Starting address of the range, nullptr on failure.
    //
    DRAKO_NODISCARD DRAKO_ALLOCATOR mem_page_ptr
    reserve_virtual_range(void* address, size_t bytes) noexcept
    {
        return drako::make_aligned(::VirtualAlloc(address, bytes, MEM_RESERVE, PAGE_READWRITE));
    }

    //
    // @brief       Release a previously reserved range of virtual memory.
    //
    void release_virtual_range(mem_page_ptr address) noexcept
    {
        auto error = ::VirtualFree(address, 0, MEM_RELEASE);
        DRAKO_ASSERT(error, "Failed");
    }
} // namespace drako::sys

#endif // !DRAKO_SYSTEM_MEMORY_HPP