#pragma once
#ifndef DRAKO_LOCKFREE_ARRAY_QUEUE
#define DRAKO_LOCKFREE_ARRAY_QUEUE

/// @brief   Thread-safe queue based on lockfree algorithms.
/// @author  Grassi Edoardo
/// @date    13/05/2019

#include "drako/concurrency/lockfree_pool_allocator.hpp"

#include <atomic>
#include <cassert>

#define QUEUE_SIZE 100

namespace drako::lockfree
{
    /// @brief Thread-safe lockfree queue
    template <typename T>
    class array_queue final
    {
    public:
        explicit array_queue(const std::size_t count) noexcept;
        ~array_queue() noexcept;

        array_queue(const array_queue&) = delete;
        array_queue& operator=(const array_queue&) = delete;

        array_queue(array_queue&&) = delete;
        array_queue& operator=(array_queue&&) = delete;


        bool enque(const T& in) noexcept;

        bool deque(T& out) noexcept;


    private:
        using _gen = uint32_t;

        struct _entry
        {
            T*   data;
            _gen gen;

            bool is_data(_gen current) noexcept
            {
                return (data != nullptr) && (gen == current);
            }

            bool is_free(_gen current) noexcept
            {
                return (data == nullptr) && (this->gen == current);
            }

            bool is_head(_gen current) noexcept
            {
                return (data != nullptr) && (gen == current);
            }

            bool is_tail(_gen current) noexcept
            {
                return (data == nullptr) ? gen == current : gen < current;
            }
        };

        struct _index
        {
            uint32_t idx;
            _gen     gen;

            index& operator++() noexcept
            {
                this->gen += ++idx / QUEUE_SIZE;
                this->idx = idx % QUEUE_SIZE;
                return *this;
            }
        };

        std::atomic<_index> head_hint = {};
        std::atomic<_index> tail_hint = {};

        std::atomic<_entry> buffer[QUEUE_SIZE];
        std::size_t        buffer_size;

        lockfree::object_pool<T> allocator;


        _index find_tail(_index hint) noexcept
        {
            while (!buffer[hint.idx].load(std::memory_order_relaxed).is_tail(hint.gen))
            {
                hint++;
            }
            return hint;
        }
    };


    template <typename T>
    inline array_queue<T>::array_queue(const size_t count) noexcept
        : buffer_size(count), allocator(count)
    {
        assert(count > 0);

        buffer = static_cast<std::atomic<_entry>*>(std::malloc(sizeof(T) * count));
        if (this->buffer == nullptr)
        {
            std::exit(EXIT_FAILURE);
        }

        for (auto i = 0; i <buffer_size; i++)
        {
            this->buffer[i] = nullptr;
        }
    }

    template <typename T>
    inline array_queue<T>::~array_queue() noexcept
    {
        assert(buffer != nullptr);
        std::free(buffer);
    }

    template <typename T>
    inline bool array_queue<T>::enque(const T& in) noexcept
    {
        T* obj = new (allocator.allocate(sizeof(T))) T(in);

        _entry new_item;
        _index pos = tail_hint.load(std::memory_order_relaxed);

        do
        {
            pos = find_tail(pos);
        } while (!buffer[pos.idx].compare_exchange_strong(nullptr, new_item));

        tail_hint.store(++pos, std::memory_order_relaxed);
        return true;
    }

    template <typename T>
    inline bool array_queue<T>::deque(T& out) noexcept
    {
        auto curr_head = head_hint.load();
        auto curr_tail = tail_hint.load();

        for (size_t i = curr_head; i != curr_tail; i++)
        {
            auto obj = ; // get object

            out = std::move(obj);
            allocator.deallocate(obj);
        }

        return false;
    }

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_ARRAY_QUEUE
