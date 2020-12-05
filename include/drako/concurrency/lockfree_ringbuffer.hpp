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
    /// @brief Single producer, single consumer thread-safe FIFO container.
    ///
    template <typename T, typename Al = std::allocator<T>>
    requires std::atomic<std::size_t>::is_always_lock_free class SRSWQueue
    {
        static_assert(std::atomic<std::size_t>::is_always_lock_free,
            "Required for lock-free property");

        using _al_traits = std::allocator_traits<Al>;

    public:
        using value_type     = T;
        using size_type      = std::size_t;
        using allocator_type = Al;

        constexpr SRSWQueue() noexcept(noexcept(Al()))
            : _alloc(), _size{ 0 }, _data{ nullptr }, _reader{ 0 }, _writer{ 0 } {}

        /// @brief Constructs a queue with specified capacity.
        explicit SRSWQueue(size_type size, const Al& alloc = Al())
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
        explicit SRSWQueue(Iter first, Iter last, const Al& alloc = Al()) // clang-format on
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

        ~SRSWQueue() noexcept
        {
            assert(_data != nullptr);
            for (auto i = _reader.load(); i != _writer.load(); i = _next_index(i))
                _al_traits::destroy(_alloc, _data + i);
            _al_traits::deallocate(_alloc, _data, _size);
        }

        SRSWQueue(const SRSWQueue&) = delete;
        SRSWQueue& operator=(const SRSWQueue&) = delete;

        SRSWQueue(SRSWQueue&&) = delete;
        SRSWQueue& operator=(SRSWQueue&&) = delete;


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
        /// @note Thread-safe and wait-free.
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
        Al              _alloc;
        const size_type _size;
        T*              _data;

        alignas(std::hardware_destructive_interference_size)
            std::atomic<size_type> _reader; // index of the most recently read item

        alignas(std::hardware_destructive_interference_size)
            std::atomic<size_type> _writer; // index of the most recently written item

        [[nodiscard]] constexpr size_type _next_index(size_type idx) noexcept
        {
            return (idx != _size) ? idx + 1 : 0;
        }
    };

    template <typename T, typename Al = std::allocator<T>>
    using RingBuffer = SRSWQueue<T, Al>;

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_RINGBUFFER_HPP
