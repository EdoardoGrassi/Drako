#pragma once
#ifndef DRAKO_ASYNC_READER_POOL_HPP
#define DRAKO_ASYNC_READER_POOL_HPP

#include "drako/concurrency/lockfree_ringbuffer.hpp"
#include "drako/core/container/static_vector.hpp"
#include "drako/io/input_file_handle.hpp"

#include <atomic>
#include <cassert>
#include <iostream>
#include <span>
#include <thread>
#include <type_traits>
#include <vector>

namespace drako::io
{
    class AsyncReaderPool
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

        struct Request
        {
            io::Handle           src;
            std::span<std::byte> dst;
            std::size_t          offset;
        };

        explicit AsyncReaderPool(const Args args) //std::size_t workers, std::size_t capacity)
        {
            assert(args.workers > 0);
            assert(args.submit_queue_size > 0);
            assert(args.output_queue_size > 0);

            for (auto i = 0; i < args.workers; ++i)
                _submit.emplace_back(args.submit_queue_size);

            for (auto i = 0; i < args.workers; ++i)
                _output.emplace_back(args.output_queue_size);

            _workers.reserve(args.workers);
            for (auto i = 0; i < args.workers; ++i) // create background threads
                _workers.push_back(std::thread{
                    _run, std::ref(_done), std::ref(_submit[i]), std::ref(_output[i]) });
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

        [[nodiscard]] bool submit(std::size_t worker, const Request* r)
        {
            assert(worker < std::size(_workers)); // invalid worker index

            return _submit[worker].push(r);
        }

        [[nodiscard]] bool retrive(std::size_t worker, Request* r)
        {
        }

    private:
        using _handle = native_handle_type;

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

        StaticVector<_queue, 4>  _submit;
        StaticVector<_queue, 4>  _output;
        std::vector<std::thread> _workers;
        std::atomic_flag         _done; // since c++20 is initialized to clear state

        static void _run(std::atomic_flag& done, _queue& in, _queue& out)
        {
            while (!done.test(std::memory_order::acquire))
            {
                while (const auto result = in.pop())
                {
                    const auto& request = result.value();
                    //packet.src.read(packet.dst, packet.bytes);
                    std::clog << "Thread " << std::this_thread::get_id()
                              << ": read " << request->dst.size_bytes() << " bytes.\n";

                    //std::invoke(request.callback);
                    assert(out.push(request));
                }

                // TODO: this should be supported in c++20 to avoid busy wait
                // done.wait(false, std::memory_order::acquire);
                std::this_thread::yield();
            }
        }
    };

} // namespace drako::io

#endif // !DRAKO_ASYNC_READER_POOL_HPP