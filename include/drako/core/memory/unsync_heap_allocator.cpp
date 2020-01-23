#include "drako/core/memory/unsync_heap_allocator.hpp"

#include <cmath>
#include <memory>

#include "drako/core/memory/memory_core.hpp"
#include "drako/math/utility.hpp"

#include "drako/devel/assertion.hpp"

namespace drako
{
    unsync_heap_allocator::unsync_heap_allocator(size_t bytes_) noexcept
    {
        DRAKO_PRECON(bytes_ > 0);

        _base = std::malloc(bytes_);
        if (_base == nullptr)
            std::exit(EXIT_FAILURE);

        #if DRAKO_API_DEBUG
        {
            fill_garbage_memory(_base, bytes_);
        }
        #endif

        for (auto list_head : _list)
            list_head = nullptr;

        auto first_header = static_cast<heap_block*>(_base);
        first_header->prev_adjacent = nullptr;
        first_header->next_adjacent = nullptr;
        first_header->next_free = nullptr;
        first_header->payload_size = bytes_ - sizeof(heap_block);

        insert_head_block(first_header);
    }


    unsync_heap_allocator::~unsync_heap_allocator() noexcept
    {
        DRAKO_ASSERT(_base != nullptr);
        std::free(_base);
    }


    void* unsync_heap_allocator::allocate(size_t bytes_) noexcept
    {
        DRAKO_PRECON(bytes_ > 0);

        // Iterate on available buckets starting from closer size approximation
        for (auto i = get_bucket_index(bytes_); i < LISTS_COUNT; ++i)
        {
            if (_list[i] != nullptr)
            {
                const auto old_block = _list[i];
                _list[i] = old_block->next_free;

                old_block->next_state = false;

                // Remove block
                const auto alloc_block = split_block(old_block, bytes_);
                return alloc_block->get_payload_ptr();
            }
        }
        // No suitable block found
        return nullptr;
    }


    void* unsync_heap_allocator::allocate(size_t bytes_, size_t alignment_) noexcept
    {
        DRAKO_PRECON(bytes_ > 0);
        DRAKO_PRECON(is_valid_alignment(alignment_));

        bytes_ += alignment_;

        // Iterate on available buckets starting from closer size approximation
        for (auto i = get_bucket_index(bytes_); i < LISTS_COUNT; ++i)
        {
            if (_list[i] != nullptr)
            {
                auto old_block = _list[i];
                _list[i] = old_block->next_free;

                // Remove block
                auto allocated_block = split_block(old_block, bytes_);
                // TODO: align pointer
                return allocated_block->get_payload_ptr();
            }
        }
        // No suitable block found
        return nullptr;
    }


    void unsync_heap_allocator::deallocate(void* DRAKO_HPP_RESTRICT _ptr) noexcept
    {
        DRAKO_ASSERT(_ptr != nullptr);

        auto freed_block = heap_block::get_header_ptr(_ptr);
        merge_block(freed_block);
        // TODO: update linked lists
    }


    inline constexpr size_t get_bucket_index(size_t _size) noexcept
    {
        // TODO: end impl
        return std::min(LISTS_COUNT - 1, drako::ceil_to_pow2(_size));
    }


    inline void unsync_heap_allocator::insert_head_block(heap_block* header_) noexcept
    {
        DRAKO_ASSERT(header_ != nullptr);

        const auto bucket_idx = get_bucket_index(header_->payload_size);
        header_->next_free = _list[bucket_idx];
        _list[bucket_idx] = header_;
    }


    inline unsync_heap_allocator::heap_block*
        unsync_heap_allocator::split_block(heap_block* current_, size_t size_) noexcept
    {
        const auto new_payload_size = current_->payload_size - (size_ + sizeof(heap_block));
        if (new_payload_size >= MIN_BLOCK_SIZE)
        {
            // Split blocks
            auto new_header = static_cast<heap_block*>(offset_pointer(current_->get_payload_ptr(), size_));
            new_header->payload_size = new_payload_size;
            new_header->prev_adjacent = current_;
            new_header->next_adjacent = current_->next_adjacent;
            current_->next_adjacent = new_header;

            insert_head_block(new_header);
        }
    }


    inline unsync_heap_allocator::heap_block*
        unsync_heap_allocator::merge_block(heap_block* curr_) noexcept
    {
        if (curr_->prev_adjacent->)

        // No free blocks
        {
            return curr_;
        }
    }
}
