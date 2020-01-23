#pragma once
#ifndef DRAKO_LOCKFREE_WORKSTEAL_BOUND_DEQUE
#    define DRAKO_LOCKFREE_WORKSTEAL_BOUND_DEQUE

#    include <atomic>
#    include <memory>

namespace drako
{

    namespace concurrency
    {
        namespace lockfree
        {
            template <typename T, typename Allocator = std::allocator<T>>
            class worksteal_bound_deque
            {
            public:
                explicit worksteal_bound_deque(size_t size) noexcept;

                worksteal_bound_deque(const worksteal_bound_deque&) = delete;
                worksteal_bound_deque& operator=(const worksteal_bound_deque&) = delete;

                worksteal_bound_deque(worksteal_bound_deque&&) = delete;
                worksteal_bound_deque& operator=(worksteal_bound_deque&&) = delete;


                void push_front();

                void pop_front();

            private:

                const size_t _size;
                std::atomic<size_t> _head;
                std::atomic<size_t> _tail;
            };
        }
    }
}

#endif // !DRAKO_LOCKFREE_WORKSTEAL_BOUND_DEQUE