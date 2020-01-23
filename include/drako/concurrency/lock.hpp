#pragma once
#ifndef DRAKO_LOCK_HPP
#define DRAKO_LOCK_HPP

#include <atomic>
#include <thread>

#include "core/compiler_macros.hpp"

namespace drako::concurrency
{
    // CLASS
    // Busy waiting lock based on TTAS locking scheme.
    class spin_lock final
    {
    public:

        DRAKO_FORCE_INLINE explicit constexpr spin_lock() noexcept = default;

        spin_lock(const spin_lock&) = delete;
        spin_lock& operator=(const spin_lock&) = delete;

        // Blocks until current execution context acquires the lock.
        // Current execution context shouldn't already hold the lock, otherwise behaviour is undefined.
        //
        DRAKO_FORCE_INLINE void lock() noexcept
        {
            for (;;)
            {
                // spin on cached memory to avoid memory bus contention
                while (!_lock.load(std::memory_order_acquire));

                // try acquisition
                if (_lock.exchange(false, std::memory_order_acq_rel))
                {
                    return;
                }
            }
        }

        // Tries once to acquire the lock.
        //
        DRAKO_NODISCARD DRAKO_FORCE_INLINE bool try_lock() noexcept
        {
            // set locked state and return previous
            return _lock.exchange(false, std::memory_order_acq_rel);
        }

        // Releases the lock.
        // Current execution context must hold the lock, otherwise behaviour is undefined.
        //
        DRAKO_FORCE_INLINE void unlock() noexcept
        {
            // set unlocked state and broadcast memory update
            _lock.store(true, std::memory_order_release);
        }

    private:

        // true = unlocked, false = locked
        std::atomic<bool> _lock{ true };
    };


    // CLASS
    // Busy waiting lock that supports multiple acquisitions from the same thread.
    class reentrant_spin_lock final
    {
    public:

        explicit constexpr reentrant_spin_lock() noexcept = default;

        // Blocks until current execution context acquires the lock.
        // 
        inline void lock() noexcept
        {
            auto id = std::this_thread::get_id();
            if (_owner.load(std::memory_order_acquire) == id)
            {
                ++_counter;
            }
            for (;;)
            {
                while (_owner.load() != std::thread::id());

                if (_owner.exchange(id, std::memory_order_acq_rel) == std::thread::id())
                {
                    return;
                }
            }
        }

        DRAKO_NODISCARD inline bool try_lock() noexcept
        {
            
        }

        // Releases the lock.
        // Current execution context must hold the lock, otherwise behaviour is undefined.
        //
        inline void unlock() noexcept
        {
            --_counter;
            if (_counter == 0)
            {
                _owner.store(std::thread::id(), std::memory_order_release);
            }
        }

    private:

        std::atomic<std::thread::id>    _owner{};
        uint32_t                        _counter{ 0 };
    };


    class read_write_lock final
    {
    public:

    private:
    };


    // CLASS
    // Busy waiting lock that allows multiple threads.
    class spin_semaphore final
    {
    public:

        DRAKO_FORCE_INLINE explicit spin_semaphore(uint16_t thread_count) noexcept
            : _capacity(thread_count)
        {
            // TODO: assert _capacity != 0
        }

        DRAKO_FORCE_INLINE void lock() noexcept
        {
            for (;;)
            {
                while (_counter.load(std::memory_order_acquire) > _capacity);

                if (_counter.fetch_add(1, std::memory_order_acq_rel) <= _capacity)
                {

                }
            }
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE bool try_lock() noexcept
        {

        }

        DRAKO_FORCE_INLINE void unlock() noexcept
        {

        }


    private:

        const uint16_t          _capacity;
        std::atomic<uint16_t>   _counter{ 0 };
    };

} // namespace drako::concurrency

#endif // !DRAKO_LOCK_HPP
