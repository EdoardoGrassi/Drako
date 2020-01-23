//
// \brief   FIFO collection implemented with a ring buffer.
// \author  Grassi Edoardo
//

#ifndef DRAKO_ARRAY_QUEUE_HPP
#define DRAKO_ARRAY_QUEUE_HPP
#pragma once

#include <memory>
#include <type_traits>

#include "core/dk_compiler_spec.hpp"

#if defined (DRKAPI_DEBUG)
#   include "development/dk_assertion.hpp"
#endif

namespace drako
{
    // TEMPLATE CLASS
    // Queue container backed by a ring buffer.
    template <typename T, size_t FixedSize = 0>
    class ArrayQueue final
    {
    public:

        explicit ArrayQueue() noexcept;
        ~ArrayQueue() noexcept
        {
            if (this->_buffer != nullptr)
            {
                std::free(this->_buffer);
            }
        }

        ArrayQueue(ArrayQueue const&) = delete;
        ArrayQueue& operator=(ArrayQueue const&) = delete;

        ArrayQueue(ArrayQueue&&) noexcept
            : _size(rhs.size), _buffer(rhs._buffer)
        {
            rhs._buffer = nullptr;
        }
        ArrayQueue& operator=(ArrayQueue&&) noexcept
        {
            if (this != &rhs)
            {
                this->_buffer = rhs._buffer;
                this->size = rhs.size;
                rhs._buffer = nullptr;
            }
        }

        // Checks if the queue is empty.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
            bool empty() const noexcept
        {
            return this->_head == this->_tail;
        }

        // Checks if the queue is full.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
            bool full() const noexcept
        {
            return this->_head == this->next_item(this->_tail);
        }

        // Discards the item at the front.
        bool discard_front() noexcept;

        bool discard_back() noexcept;

        // Removes the item at the front.
        bool dequeue(T& result) noexcept
        {
            if (this->empty())
            {
                return false;
            }
            else
            {
                result = this->_buffer[this->_head];
                this->_head = this->next_item(this->_head);
                return true;
            }
        }

        // Inserts at the end of the queue.
        bool enqueue(T const& value) noexcept
        {
            static_assert(std::is_nothrow_copy_assignable_v<T>,
                          "<T> move constructor must be noexcept");

            if (this->full())
            {
                return false;
            }
            else
            {
                this->_buffer[this->_tail] = value;
                this->_tail = this->next_item(this->_tail);
                return true;
            }
        }

        // Inserts at the end of the queue.
        bool enqueue(T&& value) noexcept
        {
            static_assert(std::is_nothrow_move_assignable_v<T>,
                          "<T> move constructor must be noexcept");

            if (this->full())
            {
                return false;
            }
            else
            {
                this->_buffer[this->_tail] = std::move(value);
                this->_tail = this->next_item(this->_tail);
                return true;
            }
        }

        // Returns a copy of the first enqueued item.
        DRAKO_NODISCARD bool peek_head(T& result) noexcept;

        // Returns a copy of the last enqueued item.
        DRAKO_NODISCARD bool peek_tail(T& result) noexcept;

        // Number of queued items.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
            size_t size() const noexcept
        {
            return 
        }

        // \brief Max number of items that can be queued.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
            size_t capacity() const noexcept
        {
            return FixedSize;
        }

    private:

        T       _buffer[FixedSize]; // Support buffer
        size_t  _head = 0;          // Index of current head
        size_t  _tail = 0;          // Index of current tail

        DRAKO_NODISCARD constexpr size_t next_item(size_t idx) const noexcept
        {
            return (idx + 1) % this->_size;
        }
    };


    template <typename T, typename TAlloc = std::allocator<T>>
    class ArrayQueue<T, 0, TAlloc> final
    {
    public:

        explicit ArrayQueue() = delete;
        explicit ArrayQueue(const size_t size) noexcept;
    };

    template <typename T>
    inline ArrayQueue<T>::ArrayQueue(const size_t size)
        : _size(size)
    {
        DRAKO_PRECON(size > 0);

        this->_buffer = static_cast<T*>(std::malloc(size * sizeof(T)));
        if (this->_buffer == nullptr)
        {
            std::exit(EXIT_FAILURE);
        }
    }
}

#endif // !DRAKO_ARRAY_QUEUE_HPP

