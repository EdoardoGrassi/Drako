#pragma once
#ifndef DRAKO_LOCKFREE_DEQUEUE_HPP
#define DRAKO_LOCKFREE_DEQUEUE_HPP

#include <atomic>
#include <memory>

namespace drako::lockfree
{
    template <typename T, typename Al = std::allocator<T>> // clang-format off
    requires std::atomic<std::size_t>::is_always_lock_free
    class DEQueue // clang-format on
    {
        static_assert(ATOMIC_LONG_LOCK_FREE == 2, "Not lockfree.");

    public:
        explicit DEQueue(std::size_t capacity);

        DEQueue(const DEQueue&) = delete;
        DEQueue& operator=(const DEQueue&) = delete;

        DEQueue(DEQueue&&) = delete;
        DEQueue& operator=(DEQueue&&) = delete;


        bool enque(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            const auto old_head = _head.load();
            const auto old_tail = _tail.load();
            const auto size     = old_head - old_tail;
            if (size >= _size) // no space left in the buffer
                [[unlikely]] return false;

            _data[old_head] = value;
            _head.store(old_head + 1);
        }

        bool deque(T& value) noexcept
        {
            const auto bottom = _head.load();
            const auto old_top = _tail.load();

        }

        bool steal(T& value) noexcept
        {
            const auto old_tail = _tail.load();
            const auto old_head = _head.load();
            const auto size     = old_head - old_tail;
            if (size <= 0) // no item to steal
                return false;
            const auto v = _data[old_tail];
            if (_tail.compare_exchange_strong(old_tail, old_tail + 1))
            {
                value = v;
                return true;
            }
            return false;
        }

    private:
        const std::size_t        _size;
        std::atomic<std::size_t> _head;
        std::atomic<std::size_t> _tail;
        T                        _data[100];
    };

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_DEQUEUE_HPP