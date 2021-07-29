/// Lock-free single producer, single consumer queue
/// also known as ring buffer.
///
/// @author Edoardo Grassi

#pragma once
#ifndef DRAKO_LOCKFREE_RINGBUFFER_HPP
#define DRAKO_LOCKFREE_RINGBUFFER_HPP

#include <atomic>
#include <cassert>
#include <iterator>
#include <memory> // std::allocator_traits
#include <new>
#include <optional>

namespace drako::lockfree
{
    namespace
    {
        [[nodiscard]] static constexpr std::size_t queue_next_slot_index(
            std::size_t index, std::size_t size) noexcept
        {
            assert(index < size);
            return (++index != size) ? index : 0;
        }
        static_assert(queue_next_slot_index(0, 10) == 1);
        static_assert(queue_next_slot_index(9, 10) == 0);


        [[nodiscard]] static constexpr std::size_t queue_free_slots_count(
            std::size_t head, std::size_t tail, std::size_t size) noexcept
        {
            assert(head < size);
            assert(tail < size);
            const auto ret = head - tail - 1;
            return (tail >= head) ? size + ret : ret;
            //return ((tail >= head) ? size + head - tail : head - tail) - 1;
        }
        static_assert(queue_free_slots_count(0, 0, 10) == 9);
        static_assert(queue_free_slots_count(9, 9, 10) == 9);
        static_assert(queue_free_slots_count(0, 9, 10) == 0);
        //static_assert(queue_free_slots_count(9, 0, 10) == 10 - 2);


        [[nodiscard]] static constexpr std::size_t queue_used_slots_count(
            std::size_t head, std::size_t tail, std::size_t size) noexcept
        {
            assert(head < size);
            assert(tail < size);
            const auto ret = tail - head;
            return (tail >= head) ? ret : size + ret;
        }
        static_assert(queue_used_slots_count(0, 0, 10) == 0);
        static_assert(queue_used_slots_count(9, 9, 10) == 0);
        static_assert(queue_used_slots_count(0, 9, 10) == 9);
    } // namespace



    /// @brief Single producer, single consumer thread-safe FIFO container.
    ///
    template <typename T, typename Al = std::allocator<T>>
    requires std::atomic<std::size_t>::is_always_lock_free class SR_SW_Queue
    {
        static_assert(std::atomic<std::size_t>::is_always_lock_free,
            "Required for lock-free property");

        using _al_traits = std::allocator_traits<Al>;

    public:
        using value_type     = T;
        using size_type      = std::size_t;
        using allocator_type = Al;

        /*
        constexpr SR_SW_Queue() noexcept(noexcept(Al()))
            : _alloc(), _size{ 256 }, _data{ _al_traits::allocate(_alloc, _size) }
        {
        }*/

        /*
        constexpr SR_SW_Queue() noexcept(noexcept(Al()))
            : SR_SW_Queue{ 256 }
        {
        }
        */

        /// @brief Constructs a queue with specified capacity.
        ///
        /// @param[in] size  Maximum capacity.
        /// @param[in] alloc Dedicated allocator.
        ///
        explicit SR_SW_Queue(const std::size_t size, const Al& alloc = Al())
            : _alloc{ alloc }
            , _size{ size + 1 } // needed to resolve the ambiguity of full/empty buffer
            , _data{ _al_traits::allocate(_alloc, _size) }
            , _reader{ .used_slots = 0 }
            , _writer{ .free_slots = _size - 1 }
        {
            assert(size > 0);
            assert(_reader.head_index == 0);
            assert(_writer.tail_index == 0);
            assert(_reader.used_slots == 0);
            assert(_writer.free_slots == size);
        }

        /*
#if __cpp_lib_concepts
        template <std::input_iterator Iter>
#else
        template <typename Iter>
#endif
        explicit SR_SW_Queue(Iter first, Iter last, const Al& alloc = Al()) // clang-format on
            : _alloc{ alloc }
            , _size{ std::distance(first, last) + 1 }
            , _data{ _al_traits::allocate(_alloc, _size) }
            , _reader{ .used_slots = std::distance(first, last) }
            , _writer{ .free_slots = _size - std::distance(first, last) }
        {
            assert(_size > 0);
            for (auto iter = first; iter != last; std::next(iter))
                _al_traits::construct(_alloc, *iter);
        }
        */

        ~SR_SW_Queue() noexcept
        {
            assert(_data != nullptr);

            const auto head = _reader.head_index.load();
            const auto tail = _writer.tail_index.load();

            // destroy any item left in the queue
            for (auto i = head; i != tail; i = _queue_next_slot_index(i))
                _al_traits::destroy(_alloc, _data + i);

            _al_traits::deallocate(_alloc, _data, _size);
        }

        SR_SW_Queue(const SR_SW_Queue&) = delete;
        SR_SW_Queue& operator=(const SR_SW_Queue&) = delete;

        SR_SW_Queue(SR_SW_Queue&&) = delete;
        SR_SW_Queue& operator=(SR_SW_Queue&&) = delete;


        /// @brief Inserts an element in the queue.
        ///
        /// @param[in] value Source value.
        ///
        /// @return True if the value was inserted, false otherwise.
        ///
        /// @note Thread-safe and wait-free.
        ///
        bool enque(const T& value) noexcept requires std::is_copy_constructible_v<T>
        {
            const auto tail = _writer.tail_index.load();
            if (_writer.free_slots == 0) // there is no space, synchronize cache
            {
                const auto head    = _reader.head_index.load();
                _writer.free_slots = _queue_free_slots_count(head, tail);
                if (_writer.free_slots == 0) // still no space
                    return false;
            }
            assert(_writer.free_slots > 0);

            _al_traits::construct(_alloc, _data + tail, value);

            // commit transaction
            _writer.tail_index.store(_queue_next_slot_index(tail));
            --_writer.free_slots;
            return true;
        }


        /// @brief Inserts a batch of elements in the queue.
        /// @tparam    It    Input iterator type.
        /// @param[in] first Begin of the range.
        /// @param[in] last  End of the range.
        /// @return The number of values successfully inserted.
        ///
        /// @note Thread-safe and wait-free for concurrent execution
        ///       with a single reader thread.
        ///
        /// @note This function could be more performant than a repeated
        ///       usage of the version that operates on a single element.
        ///
        template <typename It>
        [[nodiscard]] std::size_t enque(It first, It last) noexcept
#if __cpp_lib_concepts
            requires std::input_iterator<It>&& std::is_copy_constructible_v<T>
#endif
        {
            return 0;
        }

        /*
        /// @brief Inserts an element in the queue.
        ///
        /// @param[in] value Source value.
        ///
        /// @note Thread-safe and wait-free.
        ///
        bool push(T&& value) noexcept requires std::is_move_constructible_v<T>
        {
            const auto writer = _writer.load();
            const auto reader = _reader.load();
            if (const auto i = _next_index(writer); i != reader)
            {
                _al_traits::construct(_alloc, _data + i, value);
                _writer.store(i);
                return true;
            }
            return false;
        }

        template <typename... Args> // clang-format off
        requires std::is_constructible_v<T, Args...>
        bool emplace(Args&&... args) noexcept // clang-format on
        {
            const auto writer = _writer.load();
            const auto reader = _reader.load();
            if (const auto i = _next_index(writer); i != reader)
            {
                _al_traits::construct(_alloc, _data + i, std::forward<Args>(args)...);
                _writer.store(i);
                return true;
            }
            return false;
        }

        /// @brief Removes an element from the queue.
        ///
        /// @return Value from the queue, or nothing.
        ///
        /// @note Thread-safe and wait-free for concurrent execution
        /// of a single reader and a single writer.
        ///
        [[nodiscard]] std::optional<T> pop() noexcept
            requires std::is_move_assignable_v<T> ||
            std::is_copy_assignable_v<T>
        {
            const auto writer = _writer.load();
            const auto reader = _reader.load();
            if (reader != writer)
            {
                const auto value = std::make_optional(std::move(_data[reader]));
                _al_traits::destroy(_alloc, _data + reader);
                _reader.store(_next_index(reader));
                return value;
            }
            return {};
        }*/


        /// @brief Removes an element from the queue.
        ///
        /// @param[out] value Destination for the element to remove.
        ///
        /// @return True if an item has been removed, false otherwise.
        ///
        /// @note Thread-safe and wait-free for concurrent execution
        ///       with a single reader thread.
        ///
        [[nodiscard]] bool deque(T& value) noexcept
        {
            const auto head = _reader.head_index.load();
            if (_reader.used_slots == 0)
            {
                const auto tail    = _writer.tail_index.load();
                _reader.used_slots = _queue_used_slots_count(head, tail);
                if (_reader.used_slots == 0)
                    return false;
            }
            assert(_reader.used_slots > 0);

            value = std::move(_data[head]);
            _al_traits::destroy(_alloc, _data + head);

            // commit transaction
            _reader.head_index.store(_queue_next_slot_index(head));
            --_reader.used_slots;
            return true;
        }



        /// @brief Removes a batch of elements from the queue.
        /// @tparam    It    Output iterator type.
        /// @param[in] first Begin of the range.
        /// @param[in] last  End of the range.
        /// @return The number of values successfully removed.
        ///
        /// @note Thread-safe and wait-free for concurrent execution
        ///       with a single writer thread.
        ///
        /// @note This function could be more performant than a repeated
        ///       usage of the version that operates on a single element.
        ///
        template <typename It>
        [[nodiscard]] std::size_t deque(It first, It last) noexcept
#if __cpp_lib_concepts
            requires std::output_iterator<It, T>&& std::is_copy_constructible_v<T>
#endif
        {
            return 0;
        }



        /// @brief Checks whether the queue is empty.
        ///
        /// @return True if the queue is empty, false otherwise.
        ///
        /// @warning Not thread-safe.
        ///
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return _reader.head.load() == _writer.tail.load();
        }

        /// @brief Number of element in the queue.
        ///
        /// @return The number of elements currently in the queue.
        ///
        /// @warning Not thread-safe.
        ///
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            const auto head = _reader.head_index.load();
            const auto tail = _writer.tail_index.load();

            return _queue_used_slots_count(head, tail);
        }

        /// @brief Capacity of the queue.
        [[nodiscard]] constexpr std::size_t capacity() const noexcept { return _size - 1; }

        /// @brief Max capacity allowed by the implementation.
        [[nodiscard]] constexpr std::size_t max_size() const noexcept
        {
            return _al_traits::max_size(_alloc);
        }

    private:
        struct _reader_cached_state
        {
            std::atomic<std::size_t> head_index = 0; // index of the most recently read item
            std::size_t              used_slots;     // number of occupied slots in the queue
        };

        struct _writer_cached_state
        {
            std::atomic<std::size_t> tail_index = 0; // index of the most recently written item
            std::size_t              free_slots;     // number of free slots in the queue
        };


        Al                _alloc;
        const std::size_t _size;
        T*                _data;

        /*vvv avoid false cache sharing between reader and writer vvv*/

        alignas(std::hardware_destructive_interference_size)
            _reader_cached_state _reader;

        alignas(std::hardware_destructive_interference_size)
            _writer_cached_state _writer;


        [[nodiscard]] constexpr auto _queue_next_slot_index(
            std::size_t index) const noexcept
        {
            return queue_next_slot_index(index, _size);
        }

        [[nodiscard]] constexpr auto _queue_free_slots_count(
            std::size_t head, std::size_t tail) const noexcept
        {
            return queue_free_slots_count(head, tail, _size);
        }

        [[nodiscard]] constexpr auto _queue_used_slots_count(
            std::size_t head, std::size_t tail) const noexcept
        {
            return queue_used_slots_count(head, tail, _size);
        }
    };

    template <typename T, typename Al = std::allocator<T>>
    using RingBuffer = SR_SW_Queue<T, Al>;

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_RINGBUFFER_HPP
