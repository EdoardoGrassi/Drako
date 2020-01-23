#pragma once
#ifndef DRAKO_LOCKFREE_LINEAR_ALLOCATOR_HPP
#define DRAKO_LOCKFREE_LINEAR_ALLOCATOR_HPP

//
//  \brief      Thread safe linear allocator implemented without locks.
//  \author     Grassi Edoardo
//

#include <atomic>

#include "drako/core/dk_memory_utility.hpp"
#include "drako/core/compiler_macros.hpp"

#if DRKAPI_DEBUG
#   include "drako/development/dk_assertion.hpp"
#endif


namespace drako::concurrency
{
    // \brief   Thread safe linear allocator.
    //
    template <typename TAlloc = std::allocator<void>>
    class lockfree_linear_allocator
    {
        static_assert(std::atomic<void*>::is_always_lock_free, "Required to guarantee lock-free property");

    public:

        // \brief       Constructor.
        // \param[in]   size    Number of bytes of memory reserved for the allocator.
        //
        explicit lockfree_linear_allocator(
            const size_t size,
            TAlloc const& allocator = std::allocator<void>()) noexcept;

        // \brief   Destructor.
        //
        ~lockfree_linear_allocator() noexcept;


        lockfree_linear_allocator(const lockfree_linear_allocator&) = delete;
        lockfree_linear_allocator& operator=(const lockfree_linear_allocator&) = delete;

        lockfree_linear_allocator(lockfree_linear_allocator&&) = delete;
        lockfree_linear_allocator& operator=(lockfree_linear_allocator&&) = delete;


        // \brief       Allocates memory from the allocator.
        // \param[in]   size    Size of the requested memory in bytes.
        // \returns     Pointer to the first byte of the allocated memory.
        // \retval      null    Pointer if the allocation fails.
        //
        DRAKO_NODISCARD DRAKO_ALLOCATOR void* allocate(
            const size_t size) noexcept;


        // \brief       Allocates memory from the allocator.
        // \param[in]   size        Size of the requested memory in bytes.
        // \param[in]   alignment   Alignment of the memory block (must be a power of 2).
        // \returns     Pointer to the first byte of the allocated memory.
        // \retval      null         The allocation failed.
        //
        DRAKO_NODISCARD DRAKO_ALLOCATOR void* allocate(
            const size_t size,
            const size_t alignment) noexcept;


        // \brief   Deallocates all currently allocated memory.
        //
        void release() noexcept;

    private:

        TAlloc const& allocator;
        size_t buffer_size;          // Size of reserved memory block
        char* _buffer;           // Pointer to the base of the stack
        std::atomic<char*> _head;    // Pointer to the current top of the stack
    };


    template <typename TAlloc>
    inline lockfree_linear_allocator<TAlloc>::lockfree_linear_allocator(
        const size_t size,
        TAlloc const& allocator) noexcept
    {
        DRAKO_PRECON(size > 0);
        DRAKO_PRECON(allocator != nullptr);

        this->buffer_size = size;

        if constexpr (TAlloc == std::allocator<void>)
        {
            this->_buffer = static_cast<char*>(_aligned_malloc(size, memory::DRKAPI_L1_CACHE_SIZE));
        }
        else
        {
            this->allocator = allocator;
            this->_buffer = static_cast<char*>(allocator.allocate(size, memory::DRKAPI_L1_CACHE_SIZE));
        }

        if (this->_buffer == nullptr)
        {
            std::exit(EXIT_FAILURE);
        }
        this->_head = this->_buffer;

        #if DRKAPI_DEBUG
        {
            fill_garbage_memory(this->_buffer, this->buffer_size);
        }
        #endif
    }


    template <typename TAlloc>
    inline lockfree_linear_allocator<TAlloc>::~lockfree_linear_allocator() noexcept
    {
        DRAKO_PRECON(this->_buffer != nullptr);
        DRAKO_PRECON(this->_head == this->_buffer,
                     "Some memory is still allocated");

        if constexpr (TAlloc == std::allocator<void>)
        {
            _aligned_free(this->_buffer);
        }
        else
        {
            this->allocator.deallocate(this->_buffer);
        }
    }

    template <typename TAlloc>
    inline void* lockfree_linear_allocator<TAlloc>::allocate(const size_t size) noexcept
    {
        DRAKO_PRECON(size > 0);

        for (;;)
        {
            auto old_head = _head.load();
            // Check if there is enough memory to satisfy request
            if ((this->buffer_size - (old_head - _buffer)) >= size)
            {
                // Reserve bytes for the allocation
                auto new_head = old_head + size;
                #if DRKAPI_DEBUG
                {
                    new_head += sizeof(memory::DRKAPI_OVERFLOW_MEMORY);
                }
                #endif

                // Try atomic exchange
                if (_head.compare_exchange_strong(old_head, new_head))
                {
                    return old_head;
                }
            }
            else
            {
                return nullptr;
            }
        }
    }


    template <typename TAlloc>
    inline void* lockfree_linear_allocator<TAlloc>::allocate(const size_t size, const size_t alignment) noexcept
    {
        DRAKO_PRECON(size > 0);
        DRAKO_PRECON(is_valid_alignment(alignment));

        for (;;)
        {
            auto old_head = _head.load();
            auto aligned_ptr = static_cast<char*>(align_up_pointer(old_head, alignment));
            // Check if there is enough memory to satisfy request
            if ((size - (aligned_ptr - this->_buffer)) >= this->buffer_size)
            {
                auto new_head = aligned_ptr + size;
                // Try atomic exchange
                if (_head.compare_exchange_strong(old_head, new_head))
                {
                    return aligned_ptr;
                }
            }
            else
            {
                return nullptr;
            }
        }
    }


    template <typename TAlloc>
    inline void lockfree_linear_allocator<TAlloc>::release() noexcept
    {
        this->_head.store(this->_buffer/*, std::memory_order_release*/);
    }
}

#endif // !DRAKO_LOCKFREE_LINEAR_ALLOCATOR_HPP

