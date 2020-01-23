#pragma once
#ifndef DRAKO_STACK_ALLOCATOR_HPP
#define DRAKO_STACK_ALLOCATOR_HPP

//
// \brief      Memory allocator designed for LIFO sequenced allocations.
// \details    Linear allocator implemented with a stack. Not thread safe.
// \author     Grassi Edoardo
//


#include <cstdint>

#include "drako/core/dk_memory_utility.hpp"
#include "drako/core/drako_api.hpp"
#include "drako/core/compiler_macros.hpp"
#include "drako/math/dk_math_utility.hpp"

#if DRKAPI_DEBUG
#   include "drako/development/dk_assertion.hpp"
#endif

namespace drako
{
    /// <summary>
    /// Memory allocator for LIFO sequenced allocations.
    /// <para>Not thread safe.</para>
    /// </summary>
    class stack_allocator final
    {
    public:

        /// <summary>Creates a stack allocator with the specified size.</summary>
        explicit stack_allocator(const size_t size) noexcept;

        // \brief   Destructor.
        //
        ~stack_allocator() noexcept;


        stack_allocator(const stack_allocator&) = delete;
        stack_allocator& operator=(const stack_allocator&) = delete;

        constexpr stack_allocator(stack_allocator&&) noexcept = delete;
        constexpr stack_allocator& operator=(stack_allocator&&) noexcept = delete;


        /// <summary>Allocates memory from the top of the stack.</summary>
        /// <param name="bytes_count">Number of requested byte. Must be greater than 0.</param>
        /// <returns>If the allocation succeeds returns a pointer to the allocated memory. Otherwise returns a null pointer.</returns>
        DRAKO_NODISCARD DRAKO_ALLOCATOR void* allocate(const size_t size) noexcept;


        /// <summary>Allocates memory from the top of the stack with the requested alignment.</summary>
        /// <param name="bytes_count">Number of requested byte. Must be greater than 0.</param>
        /// <param name="alignment">Requested memory alignment. Must be a power of 2.</param>
        /// <returns>If the allocation succeeds returns a pointer to the allocated memory. Otherwise returns a null pointer.</returns>
        DRAKO_NODISCARD DRAKO_ALLOCATOR void* allocate(
            const size_t size,
            const size_t alignment) noexcept;


        /// <summary>Deallocates memory from the top of the stack.</summary>
        /// <param name="memory_ptr">Pointer to previously allocated memory. Can't be null.</param>
        void deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept;


    private:

        struct allocation_header
        {
            uint8_t align_offset;
        };

        char* _buffer;     // Pointer to the first preallocated bytes of the stack
        char* _mem_end;       // Pointer to the last preallocated byte of the stack
        char* _head;   // Pointer to the head of the stack
    };


    inline stack_allocator::stack_allocator(const size_t size) noexcept
    {
        DRAKO_PRECON(size > 0);

        // TODO: align to cache page, add overflow buffers
        this->_buffer = static_cast<char*>(_aligned_malloc(size, memory::l1_cache_line_size));
        if (this->_buffer == nullptr)
        {
            std::exit(EXIT_FAILURE);
        }
        this->_head = this->_buffer;
        this->_mem_end = this->_buffer + size - 1;

        #if DRKAPI_DEBUG
        {
            fill_garbage_memory(this->_buffer, size);
        }
        #endif
    }


    inline stack_allocator::~stack_allocator() noexcept
    {
        DRAKO_ASSERT(this->_buffer != nullptr);
        DRAKO_ASSERT(this->_head == this->_buffer,
                     "Some memory is still allocated");

        std::free(_buffer);
    }


    inline void* stack_allocator::allocate(const size_t size) noexcept
    {
        DRAKO_PRECON(size > 0);

        auto new_current = this->_head + size + sizeof(allocation_header);
        if (new_current > this->_mem_end)
        {
            // Not enough space
            return nullptr;
        }
        // Save alignment offset
        *this->_head = 0;
        auto ptr = this->_head + sizeof(allocation_header);

        #if DRKAPI_DEBUG
        {
            // Initialize memory with garbage values
            fill_garbage_memory(ptr, size);
        }
        #endif

        this->_head = new_current;
        return ptr;
    }


    inline void* stack_allocator::allocate(const size_t size, const size_t alignment) noexcept
    {
        DRAKO_PRECON(size > 0);
        DRAKO_PRECON(is_valid_alignment(alignment));

        auto ptr = static_cast<char*>(align_up_pointer(this->_head + sizeof(allocation_header), alignment));
        auto new_current = ptr + size;
        if (new_current > this->_mem_end)
        {
            // Not enough space
            return nullptr;
        }
        // Save alignment offset
        ptr[-1] = (ptr - this->_head) - 1;

        #if DRKAPI_DEBUG
        {
            fill_garbage_memory(ptr, size);
        }
        #endif

        this->_head = new_current;

        return ptr;
    }


    inline void stack_allocator::deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept
    {
        DRAKO_PRECON(ptr != nullptr);
        DRAKO_PRECON(ptr > this->_buffer);
        DRAKO_PRECON(ptr < this->_mem_end);

        auto align_offset = *(static_cast<char*>(ptr) - sizeof(allocation_header));
        this->_head = static_cast<char*>(ptr) - align_offset;

        DRAKO_POSTCON(this->_head >= this->_buffer);
    }
}

#endif // !DRAKO_STACK_ALLOCATOR_HPP