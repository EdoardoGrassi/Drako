/// Lock-free single producer, single consumer queue
/// also known as ring buffer.
///
/// @author Edoardo Grassi

#pragma once
#ifndef DRAKO_LOCKFREE_RINGBUFFER
#define DRAKO_LOCKFREE_RINGBUFFER

#include <atomic>
#include <cassert>
#include <iterator>
#include <memory> // std::allocator_traits
#include <new>

namespace drako::lockfree
{
    template <typename T, typename Al = std::allocator<T>> // clang-format off
    requires std::atomic<std::size_t>::is_always_lock_free
    class ringbuffer final // clang-format on
    {
        static_assert(std::atomic<std::size_t>::is_always_lock_free,
            "Required for lock-free property");

        using _al_traits = std::allocator_traits<Al>;

    public:
        using size_type      = std::size_t;
        using allocator_type = Al;

        /// @brief Constructs a queue with specified capacity.
        explicit ringbuffer(size_type size, const Al& alloc = Al())
            : _alloc{ alloc }
            , _size{ size }
            , _data{ _al_traits::allocate(_alloc, _size) }
            , _reader{ 0 }
            , _writer{ 0 }
        {
            assert(_size > 0);
            // vvv not needed because allocate throws on failure
            //if (_data == nullptr)
            //    [[unlikely]] throw std::bad_alloc{ "Failed to allocate backing memory." };
        }

        /*
        template <input_iterator Iter> // clang-format off
        explicit ringbuffer(Iter first, Iter last, const Al& alloc = Al()) // clang-format on
            : _alloc{ alloc }
            , _size{ std::distance(first, last) }
            , _data{ _al_traits::allocate(_alloc, _size) }
            , _reader{ 0 }
            , _writer{ std::distance(first, last) }
        {
            assert(_size > 0);
            // vvv not needed because allocate throws on failure
            //if (_data == nullptr)
            //    [[unlikely]] throw std::bad_alloc{ "Failed to allocate backing memory." };

            // TODO: end impl
            for (auto iter = first; iter != last; std::next(iter))
                _al_traits::construct(_alloc, *iter);
        }
        */

        ~ringbuffer() noexcept
        {
            assert(_data != nullptr);
            for (auto i = _reader.load(); i != _writer.load(); i = _next_index(i))
                _al_traits::destroy(_alloc, _data + i);
            _al_traits::deallocate(_alloc, _data, _size);
        }

        ringbuffer(const ringbuffer&) = delete;
        ringbuffer& operator=(const ringbuffer&) = delete;

        ringbuffer(ringbuffer&&) = delete;
        ringbuffer& operator=(ringbuffer&&) = delete;


        /// @brief Inserts an element in the queue.
        ///
        /// @param[in] value Source value.
        ///
        /// @note Thread-safe and wait-free.
        ///
        bool push(const T& value) noexcept requires std::is_copy_constructible_v<T>
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

        template <typename... Args>
        bool emplace(Args&&... args) noexcept requires std::is_constructible_v<T, Args...>
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
        /// @param[out] value Destination of the removed element.
        ///
        /// @note Thread-safe and wait-free.
        ///
        [[nodiscard]] bool pop(T& value) noexcept requires std::is_move_assignable_v<T>
        {
            const auto writer = _writer.load();
            const auto reader = _reader.load();
            if (reader != writer)
            {
                value = std::move(_data[reader]);
                _al_traits::destroy(_alloc, _data + reader);
                _reader.store(_next_index(reader));
                return true;
            }
            return false;
        }

        /// @brief Checks whether the queue is empty.
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return _reader.load() == _writer.load();
        }


        /// @brief Checks whether the queue is full.
        //[[nodiscard]] constexpr bool full() const noexcept;

        /// @brief Number of element in the queue.
        [[nodiscard]] constexpr size_type size() const noexcept
        {
            const auto reader = _reader.load(std::memory_order::acquire);
            const auto writer = _writer.load(std::memory_order::acquire);

            return (writer >= reader) ? writer - reader
                                      : writer + _size - reader;
        }

        /// @brief Capacity of the queue.
        [[nodiscard]] constexpr size_type capacity() const noexcept { return _size; }

        /// @brief Max capacity allowed by the implementation.
        [[nodiscard]] constexpr size_type max_size() const noexcept
        {
            return _al_traits::max_size(_alloc);
        }

    private:
        Al                     _alloc;
        const size_type        _size;
        T*                     _data;
        std::atomic<size_type> _reader; // index of the most recently read item
        std::atomic<size_type> _writer; // index of the most recently written item

        [[nodiscard]] constexpr size_type _next_index(size_type idx) noexcept
        {
            return (idx != _size) ? idx + 1 : 0;
        }
    };


    template <typename T, typename Al = std::allocator<T>>
    using RingBuffer = ringbuffer<T, Al>;

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_RINGBUFFER
