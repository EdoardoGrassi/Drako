#pragma once
#ifndef DRAKO_UNSYNC_HEAP_ALLOCATOR_HPP
#define DRAKO_UNSYNC_HEAP_ALLOCATOR_HPP

//
// \author  Grassi Edoardo
//

#include <cstddef>
#include <new>

#include "drako/core/compiler.hpp"

// Minimum payload size of a standalone block.
// Partially allocated blocks won't be splitted if the resulting blocks are smaller than this limit.
#define MIN_BLOCK_SIZE 10u

// Number of different linked lists managed.
#define LISTS_COUNT 30u

namespace drako
{
    class unsync_heap_allocator
    {
    public:

        explicit unsync_heap_allocator(size_t bytes) noexcept;
        ~unsync_heap_allocator() noexcept;

        unsync_heap_allocator(const unsync_heap_allocator&) = delete;
        unsync_heap_allocator& operator=(const unsync_heap_allocator&) = delete;

        unsync_heap_allocator(unsync_heap_allocator&&) noexcept = delete;
        unsync_heap_allocator& operator=(unsync_heap_allocator&&) noexcept = delete;


        DRAKO_NODISCARD DRAKO_ALLOCATOR
            void* allocate(size_t bytes) noexcept;


        DRAKO_NODISCARD DRAKO_ALLOCATOR
            void* allocate(size_t bytes, size_t alignment) noexcept;


        DRAKO_NODISCARD DRAKO_ALLOCATOR
            void* reallocate(void* DRAKO_HPP_RESTRICT ptr, size_t bytes) noexcept;


        void deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept;

    private:

        struct heap_block final
        {
        public:

            // Allocation state of previous adjacent block
            bool prev_state : 1;

            // Allocation state of next adjacent block
            bool next_state : 1;

            // Byte size of the block payload
            uint32_t payload_size : 30;

            // Previous adjacent block
            heap_block* prev_adjacent;

            // Next adjacent block
            heap_block* next_adjacent;

            // Previous free block within the same linked list
            heap_block* prev_free;

            // Next free block within the same linked list
            heap_block* next_free;

            explicit constexpr heap_block() noexcept;

            inline constexpr void* get_payload_ptr() noexcept
            {
                return static_cast<void*>(&(this->next_free));
            }

            static inline constexpr heap_block* get_header_ptr(void* ptr) noexcept
            {
                return reinterpret_cast<heap_block*>(static_cast<std::byte*>(ptr) + offsetof(heap_block, prev_free));
            }
        };

        void*       _base;
        heap_block* _list[LISTS_COUNT];

        constexpr size_t get_bucket_index(size_t size) noexcept;

        // Insert block at the start of a linked list
        inline void insert_head_block(heap_block* header) noexcept;

        // Remove block from the start of selected linked list
        inline heap_block* remove_head_block(size_t bucket_index) noexcept;

        heap_block* split_block(heap_block* header, size_t size) noexcept;

        heap_block* merge_block(heap_block* header) noexcept;
    };
    static_assert(sizeof(unsync_heap_allocator) < std::hardware_destructive_interference_size);
}

#endif // !DRAKO_UNSYNC_HEAP_ALLOCATOR_HPP
