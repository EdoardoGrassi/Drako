#pragma once
#ifndef DRAKO_UNSYNC_POOL_ALLOCATOR_HPP
#define DRAKO_UNSYNC_POOL_ALLOCATOR_HPP

/// @file
///
/// @brief      Unsynchronized pool allocator.
/// @author     Grassi Edoardo

#include "drako/core/memory/unsync_memory_pool.hpp"
#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"

namespace drako
{
    // CLASS TEMPLATE
    // Unsynchronized allocator that recycles objects inside a preallocated pool.
    template <typename T>
    class unsync_pool_allocator : private unsync_memory_pool<sizeof(T), alignof(T)>
    {
    public:

        // \brief       Constructor.
        //
        // \param[in]   count      Maximum capacity of the pool.
        //
        explicit unsync_pool_allocator(size_t capacity) noexcept
            : unsync_memory_pool(capacity)
        {
        }
        
        unsync_pool_allocator(const unsync_pool_allocator&) = delete;
        unsync_pool_allocator& operator=(const unsync_pool_allocator&) = delete;

        unsync_pool_allocator(unsync_pool_allocator&&) noexcept = default;
        unsync_pool_allocator& operator=(unsync_pool_allocator&&) noexcept = default;


        // \brief   Allocates and constructs an object in the pool.
        //
        DRAKO_NODISCARD T* new_object() noexcept(std::is_nothrow_constructible_v<T>);


        // \brief   Destroys and deallocates an object from the pool.
        //
        void delete_object(T* ptr) noexcept(std::is_nothrow_destructible_v<T>)
        {
            ptr->~();
            deallocate(ptr);
        }


        // \brief   Checks if the allocator is full.
        //
        DRAKO_NODISCARD constexpr bool full() const noexcept;


        // \brief   Converts an object pointer to corresponding index inside the pool.
        //
        DRAKO_NODISCARD constexpr size_t ptr_to_index(T* ptr) const noexcept
        {

        }


        // \brief   Converts an object index to corresponding pointer inside the pool.
        //
        DRAKO_NODISCARD constexpr T* index_to_ptr(size_t idx) const noexcept
        {

        }
    };

} // namespace drako

#endif // !DRAKO_UNSYNC_POOL_ALLOCATOR_HPP