#pragma once
#ifndef DRAKO_ASYNC_READER_POOL_HPP
#define DRAKO_ASYNC_READER_POOL_HPP

#include "drako/concurrency/lockfree_ringbuffer.hpp"
#include "drako/core/container/static_vector.hpp"

#include <rio/input_file_handle.hpp>

#include <atomic>
#include <cassert>
#include <iostream>
#include <optional>
#include <span>
#include <thread>
#include <type_traits>
#include <vector>

namespace drako
{
    class AsyncReaderPoolInterface
    {
    public:
        struct Request
        {
            /// @brief Source open handle of the file
            rio::Handle src;

            /// @brief Destination memory buffer
            std::span<std::byte> dst;

            /// @brief Bytes offset from the start of the file
            std::size_t offset;
        };

        virtual bool submit(const Request*) noexcept;
        virtual bool retrive(Request*) noexcept;
    };


    class AsyncReaderPool : AsyncReaderPoolInterface
    {
    public:
        struct Args
        {
            /// @brief Number of worker threads.
            std::size_t workers;

            /// @brief Capacity of the input buffer of each worker.
            std::size_t submit_queue_size;

            /// @brief Capacity of the output buffer of each worker.
            std::size_t output_queue_size;
        };

        explicit AsyncReaderPool(const Args& args) //std::size_t workers, std::size_t capacity)
        {
            assert(args.workers > 0);
            assert(args.submit_queue_size > 0);
            assert(args.output_queue_size > 0);

            for (auto i = 0; i < args.workers; ++i)
                _submit_queues.emplace_back(args.submit_queue_size);

            for (auto i = 0; i < args.workers; ++i)
                _output_queues.emplace_back(args.output_queue_size);

            // create background threads
            _workers.reserve(args.workers);
            for (auto i = 0; i < args.workers; ++i)
                _workers.push_back(std::thread{
                    _run, std::ref(_done),
                    std::ref(_submit_queues[i]), std::ref(_output_queues[i]) });
        }

        ~AsyncReaderPool() noexcept
        {
            _done.test_and_set(std::memory_order::release);
            for (auto& w : _workers)
                w.join();
        }

        /*
        [[nodiscard]] bool submit(std::size_t worker,
            UniqueInputFile& src, std::span<std::byte> dst, Callable c)
        {
            assert(worker < std::size(_workers)); // invalid worker index

            const auto handle = src.native_handle();
            return _workers[worker].queue().emplace(c, handle, dst, 0);
        }

        [[nodiscard]] bool submit(std::size_t worker,
            UniqueInputFile& src, std::span<std::byte> dst, std::size_t offset, Callable c)
        {
            assert(worker < std::size(_workers)); // invalid worker index

            const auto handle = src.native_handle();
            return _workers[worker].queue().emplace(c, handle, dst, offset);
        }
        */

        [[nodiscard]] bool submit(const Request* r) noexcept
        {
            //static thread_local std::size_t worker = 0;
            //return _submit(worker++ % std::size(_workers), r);
            return _submit(0, r);
        }

        [[nodiscard]] bool retrieve(Request* r) noexcept
        {
            //static thread_local std::size_t worker = 0;
            //return _retrive(worker++ % std::size(_workers), r);
            //return _retrive(0, r);
        }


    private:
        /*
        struct _packet
        {
            _packet() = default;

            constexpr _packet(
                Callable c, _handle src, std::span<std::byte> dst, std::size_t offset)
                : callback{ c }, src{ src }, dst{ dst }, offset{ offset } {}

            _packet(const _packet&) noexcept = default;
            _packet& operator=(const _packet&) noexcept = default;

            Callable             callback;
            std::span<std::byte> dst;
            _handle              src;
            std::size_t          offset;
        };
        static_assert(std::is_nothrow_default_constructible_v<_packet>);
        static_assert(std::is_nothrow_copy_constructible_v<_packet>);
        */

        using _queue = drako::lockfree::RingBuffer<const Request*>;

        StaticVector<_queue, 4>  _submit_queues;
        StaticVector<_queue, 4>  _output_queues;
        std::vector<std::thread> _workers;
        std::atomic_flag         _done; // since c++20 is initialized to clear state

        static void _run(std::atomic_flag& done, _queue& in, _queue& out)
        {
            while (!done.test(std::memory_order::acquire))
            {
                for (const Request* request; in.deque(request);)
                {
                    //packet.src.read(packet.dst, packet.bytes);
                    std::clog << "Thread " << std::this_thread::get_id()
                              << ": read " << request->dst.size_bytes() << " bytes.\n";

                    //std::invoke(request.callback);
                    assert(out.enque(request));
                }

                // TODO: this should be supported in c++20 to avoid busy wait
                // done.wait(false, std::memory_order::acquire);
                std::this_thread::yield();
            }
        }

        [[nodiscard]] bool _submit(const std::size_t worker, const Request* r)
        {
            assert(worker < std::size(_workers));
            assert(r);

            return _submit_queues[worker].enque(r);
        }

        [[nodiscard]] bool _retrive(const std::size_t worker, const Request* r)
        {
            assert(worker < std::size(_workers));
            assert(r);

            return _output_queues[worker].deque(r);
        }
    };


    /// @brief Mock class for testing without actual I/O operations
    class AsyncReaderPoolMock : AsyncReaderPoolInterface
    {
    };

} // namespace drako

#endif // !DRAKO_ASYNC_READER_POOL_HPP