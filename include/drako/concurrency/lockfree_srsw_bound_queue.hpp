//
// \brief   Lockfree single producer/single consumer threadsafe queue.
// \author  Grassi Edoardo
//

#pragma once
#ifndef DRAKO_LOCKFREE_SRSW_BOUND_QUEUE
#    define DRAKO_LOCKFREE_SRSW_BOUND_QUEUE

#    include <atomic>
#    include <initializer_list>
#    include <iterator>
#    include <memory>

#    include "drako/core/memory/memory_core.hpp"
#    include "drako/core/preprocessor/compiler_macros.hpp"
#    include "drako/devel/assertion.hpp"

namespace drako::lockfree
{
    template <typename T, typename Allocator = std::allocator<T>>
    class srsw_bound_queue final
    {
        static_assert(std::atomic<size_t>::is_always_lock_free,
            "Required for lock-free property");

    public:
        using size_type = size_t;
        using allocator = Allocator;

        class producer_iterator final
        {
        public:
            explicit producer_iterator(spsc_queue& queue, size_type tail) noexcept
                : _queue(queue), _tail(tail)
            {
            }

            producer_iterator(producer_iterator const&) = delete;
            producer_iterator& operator=(producer_iterator const&) = delete;

            producer_iterator(producer_iterator&&) noexcept;
            producer_iterator& operator=(producer_iterator&&) noexcept;

            bool enqueue(T const& value) noexcept;
            bool enqueue(T&& value) noexcept;

        private:
            srsw_bound_queue& _queue;
            size_type _tail;
        };

        class consumer_iterator final
        {
        public:
            explicit consumer_iterator(spsc_queue& queue, size_type head) noexcept
                : _queue(queue), _head(head)
            {
            }

            bool dequeue(T& result) noexcept;

        private:
            srsw_bound_queue& _queue;
            size_type _head;
        };

        friend class producer_iterator;
        friend class consumer_iterator;

        /// @brief Constructs a queue with minimum specified capacity.
        explicit srsw_bound_queue(size_type size, const Allocator& alloc = Allocator()) noexcept;

        explicit srsw_bound_queue(std::initializer_list<T> ilist, const Allocator& alloc = Allocator()) noexcept;

        template <typename InputIter,
            typename std::enable_if_t<
                std::is_base_of_v<std::input_iterator_tag, std::iterator_traits<InputIter>::iterator_category>>>
        explicit srsw_bound_queue(InputIter first, InputIter last, const Allocator& alloc = Allocator()) noexcept;

        /// @brief Destructor.
        ~srsw_bound_queue() noexcept;

        srsw_bound_queue(const srsw_bound_queue&) = delete;
        srsw_bound_queue& operator=(const srsw_bound_queue&) = delete;

        srsw_bound_queue(srsw_bound_queue&&) = delete;
        srsw_bound_queue& operator=(srsw_bound_queue&&) = delete;

        // Returns the producer accessor
        DRAKO_NODISCARD constexpr producer_iterator producer() noexcept;

        // Returns the consumer accessor
        DRAKO_NODISCARD constexpr consumer_iterator consumer() noexcept;

        // Returns whether the queue is empty.
        DRAKO_NODISCARD constexpr bool empty() const noexcept;

        // Returns whether the queue is full.
        DRAKO_NODISCARD constexpr bool full() const noexcept;

        /// @brief Capacity of the queue.
        DRAKO_FORCE_INLINE DRAKO_NODISCARD constexpr size_type size() const noexcept { return _size }

        /// @brief Max capacity allowed by the implementation.
        DRAKO_FORCE_INLINE DRAKO_NODISCARD constexpr size_type max_size() const noexcept
        {
            return std::numeric_limits<size_type>::max(); // TODO: this is not correct
        };

        // Number of occupied slots.
        size_type readable_count() const noexcept;

        // Number of free slots.
        size_type writable_count() const noexcept;

    private:
        Allocator _alloc;
        const size_type _size;
        T const* _data;
        std::atomic<size_type> _head{ 0 }; // Index of the first enqueued item
        std::atomic<size_type> _tail{ 0 }; // Index of the last enqueued item

        DRAKO_FORCE_INLINE DRAKO_NODISCARD static constexpr size_type
        round_size_to_power(size_type requested_size) noexcept
        {
        }

        DRAKO_FORCE_INLINE DRAKO_NODISCARD static constexpr size_type
        next_item_index(size_type idx) noexcept
        {
            return (idx + 1) & _size;
        }
    };


    template <typename T, typename Allocator>
    inline srsw_bound_queue<T, Allocator>::srsw_bound_queue(
        size_t size, const Allocator& alloc = Allocator()) noexcept
        : _alloc(alloc),
          _size(size),
          _data(static_cast<T*>(std::allocator_traits<Allocator>::allocate(_alloc, _size)))
    {
        DRAKO_ASSERT(_size > 0);
        if (DRAKO_UNLIKELY(_data == nullptr))
            std::exit(EXIT_FAILURE);
    }

    template <typename T, typename Allocator>
    inline srsw_bound_queue<T, Allocator>::srsw_bound_queue(
        std::initializer_list<T> ilist, const Allocator& alloc = Allocator()) noexcept
        : srsw_bound_queue(ilist.begin(), ilist.end(), alloc) {}

    template <typename T, typename Allocator>
    template <typename InputIter>
    inline srsw_bound_queue<T, Allocator>::srsw_bound_queue(InputIter first, InputIter last, const Allocator& alloc) noexcept
        : _alloc(alloc),
          _size(std::distance(first, last)),
          _data(static_cast<T*>(std::allocator_traits<Allocator>::allocate(_alloc, _size)))
    {
        DRAKO_ASSERT(_size > 0);
        if (DRAKO_UNLIKELY(_data == nullptr))
            std::exit(EXIT_FAILURE);

        // TODO: end impl
        for (auto iter = first; iter != last; std::next(iter))
            std::allocator_traits<Allocator>::construct(_alloc, *iter);
    }


    template <typename T, typename Allocator>
    inline srsw_bound_queue<T, Allocator>::~srsw_bound_queue() noexcept
    {
        DRAKO_ASSERT(_data != nullptr);
        for (auto it = _head; it != _tail; it = next_item_index(it))
            std::allocator_traits<Allocator>::destroy(_alloc, _data + it);
        std::free(_data);
    }

    template <typename T, typename Allocator>
    inline constexpr srsw_bound_queue<T, Allocator>::producer_iterator
    srsw_bound_queue<T, Allocator>::producer() noexcept
    {
        return producer_iterator(*this, _tail.load(std::memory_order_acquire));
    }

    template <typename T, typename Allocator>
    inline constexpr srsw_bound_queue<T, Allocator>::consumer_iterator
    srsw_bound_queue<T, Allocator>::consumer() noexcept
    {
        return consumer_iterator(*this, _head.load(std::memory_order_acquire));
    }

    template <typename T, typename Allocator>
    DRAKO_FORCE_INLINE constexpr bool srsw_bound_queue<T, Allocator>::empty() const noexcept
    {
        const auto head = _head.load();
        const auto tail = _tail.load();
        return head == tail;
    }

    template <typename T, typename Allocator>
    DRAKO_FORCE_INLINE constexpr bool srsw_bound_queue<T, Allocator>::full() const noexcept
    {
        // TODO: end impl
    }

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_SRSW_BOUND_QUEUE
