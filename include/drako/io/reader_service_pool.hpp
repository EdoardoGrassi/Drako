#pragma once
#ifndef DRAKO_IO_READER_SERVICE_POOL
#define DRAKO_IO_READER_SERVICE_POOL

#include "drako/concurrency/lockfree_ringbuffer.hpp"
#include "drako/io/input_file_handle.hpp"

#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

namespace drako::io
{
    class ReaderPool
    {
    public:
        [[nodiscard]] explicit ReaderPool(std::size_t capacity)
            : ReaderPool{ std::thread::hardware_concurrency(), capacity }
        {
        }

        [[nodiscard]] explicit ReaderPool(std::size_t workers, std::size_t capacity)
        {
            assert(workers > 0);
            assert(capacity > 0);

            for (auto i = 0; i < workers; ++i) // create work buffers
                _queues.emplace_back(capacity);
            for (auto i = 0; i < workers; ++i) // create worker threads
                _workers.emplace_back(_queues[i]);
        }

        [[nodiscard]] bool submit(input_file_handle&& src, std::byte* dst, std::size_t bytes)
        {
            const auto handle = src.native_handle();
            const auto queue  = _select_queue();
            return _queues[queue].emplace(handle, dst, bytes, 0);
        }

    private:
        using _handle = native_handle_type;

        struct _packet
        {
            _handle     src;
            std::byte*  dst;
            std::size_t bytes;
            std::size_t offset;
        };
        using _queue = drako::lockfree::RingBuffer<_packet>;

        class _worker
        {
        public:
            explicit _worker(_queue& q)
                : _thread{ &_worker::_run, this, std::ref(_done), std::ref(q) } {}

            //void wakeup() noexcept;

            void stop() noexcept
            {
                _done.test_and_set(std::memory_order::release);
            }

        private:
            std::atomic_flag _done; // since c++20 is initialized to clear state
            std::thread      _thread;

            void _run(std::atomic_flag& done, _queue& q)
            {
                while (!done.test(std::memory_order::acquire))
                {
                    _packet packet;
                    while (q.pop(packet))
                    {
                        //packet.src.read(packet.dst, packet.bytes);
                        std::clog << "Thread " << std::this_thread::get_id()
                                  << ": read " << packet.bytes << " bytes.\n";
                    }

                    // TODO: this should be supported in c++20 to avoid busy wait
                    // done.wait(false, std::memory_order::acquire);
                    std::this_thread::yield();
                }
            }
        };

        std::vector<_queue>      _queues;
        std::vector<_worker>     _workers;
        std::atomic<std::size_t> _counter;

        static_assert(decltype(_counter)::is_always_lock_free,
            "Required property for an efficient implementation.");

        [[nodiscard]] std::size_t _select_queue() noexcept
        {
            return _counter.fetch_add(1, std::memory_order::relaxed) % std::size(_queues);
        }
    };

} // namespace drako::io

#endif // !DRAKO_IO_READER_SERVICE_POOL