#pragma once
#ifndef DRAKO_LOCKFREE_ARRAY_QUEUE
#define DRAKO_LOCKFREE_ARRAY_QUEUE

//
// @brief   Thread-safe queue based on lockfree algorithms.
// @author  Grassi Edoardo
// @date    13/05/2019
//

#include <atomic>

#include "core/compiler_macros.hpp"
#include "core/lockfree_pool_allocator.hpp"

#if DRKAPI_DEBUG
#   include "development/assertion.hpp"
#endif

#define QUEUE_SIZE 100

namespace drako::concurrency::lockfree
{
    template <typename T>
    class array_queue final
    {
    public:

        explicit array_queue(const size_t count) noexcept;
        ~array_queue() noexcept;

        array_queue(const array_queue&) = delete;
        array_queue& operator=(const array_queue&) = delete;

        array_queue(array_queue&&) = delete;
        array_queue& operator=(array_queue&&) = delete;


        bool enque(const T& in) noexcept;

        bool deque(T& out) noexcept;


    private:

        using entry_gen = uint32_t;

        struct entry
        {
            T* data;
            entry_gen gen;

            bool is_data(entry_gen curr_gen) noexcept
            {
                return (this->data != nullptr) && (this->gen == curr_gen);
            }

            bool is_free(entry_gen curr_gen) noexcept
            {
                return (this->data == nullptr) && (this->gen == curr_gen);
            }

            bool is_head(entry_gen curr_gen) noexcept
            {
                return this->data != nullptr && this->gen == curr_gen;
            }

            bool is_tail(entry_gen curr_gen) noexcept
            {
                return (this->data == nullptr) ?
                    this->gen == curr_gen : this->gen < curr_gen;
            }
        };

        struct index
        {
            uint32_t idx;
            uint32_t gen;

            index& operator++() noexcept
            {
                this->gen += ++idx / QUEUE_SIZE;
                this->idx = idx % QUEUE_SIZE;
                return *this;
            }
        };

        std::atomic<index> head_hint = {};
        std::atomic<index> tail_hint = {};

        std::atomic<entry> buffer[QUEUE_SIZE];
        size_t buffer_size;

        memory::lockfree_object_pool<T> allocator;


        index find_tail(index hint) noexcept
        {
            while (!this->buffer[hint.idx].load(std::memory_order_relaxed).is_tail(hint.gen)) { hint++; }
            return hint;
        }
    };


    template<typename T>
    inline array_queue<T>::array_queue(const size_t count) noexcept
        : buffer_size(count), allocator(count)
    {
        DRAKO_PRECON(count > 0);

        this->buffer = static_cast<std::atomic<entry>*>(std::malloc(sizeof(T) * count));
        if (this->buffer == nullptr) { std::exit(EXIT_FAILURE); }

        for (auto i = 0; i < this->buffer_size; i++)
        {
            this->buffer[i] = nullptr;
        }
    }

    template<typename T>
    inline array_queue<T>::~array_queue() noexcept
    {
        DRAKO_ASSERT(this->buffer != nullptr);
        std::free(this->buffer);
    }

    template<typename T>
    inline bool array_queue<T>::enque(const T& in) noexcept
    {
        T* obj = new (this->allocator.allocate(sizeof(T))) T(in);

        entry new_item;
        index pos = this->tail_hint.load(std::memory_order_relaxed);

        do
        {
            pos = find_tail(pos);
        }
        while (!this->buffer[pos.idx].compare_exchange_strong(nullptr, new_item));

        this->tail_hint.store(++pos, std::memory_order_relaxed);
        return true;
    }

    template <typename T>
    inline bool array_queue<T>::deque(T& out) noexcept
    {
        auto curr_head = this->head_hint.load();
        auto curr_tail = this->tail_hint.load();

        for (size_t i = curr_head; i != curr_tail; i++)
        {
            auto obj = ;// get object

            out = std::move(obj);
            this->allocator.deallocate(obj);
        }

        return false;
    }

} // namespace drako::concurrency::lockfree

#endif // !DRAKO_LOCKFREE_ARRAY_QUEUE
