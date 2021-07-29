#include "drako/concurrency/lockfree_ringbuffer.hpp"

#include <gtest/gtest.h>

#include <thread>

using namespace drako::lockfree;

GTEST_TEST(RingBuffer, Construction)
{
    using T = int;

    //RingBuffer<T> default_capacity{};
    //EXPECT_EQ(default_capacity.capacity(), 256);

    RingBuffer<T> custom_capacity{ 10'999 };
    EXPECT_EQ(custom_capacity.capacity(), 10'999);
}

GTEST_TEST(RingBuffer, SingleThreadOps)
{
    using T = int;
    RingBuffer<T> rb{ 1000 };

    const auto cycles = 1000, ops = 100;
    for (auto k = 0; k < cycles; ++k)
    {
        for (auto i = 0; i < ops; ++i)
            rb.enque(i);

        ASSERT_EQ(std::size(rb), ops);

        for (auto i = 0; i < ops; ++i)
        {
            T out;
            ASSERT_TRUE(rb.deque(out));
            ASSERT_EQ(out, i);
        }

        ASSERT_EQ(std::size(rb), 0);
    }
}

GTEST_TEST(RingBuffer, EasyMultiThreadOps)
{
    using T = int;
    RingBuffer<T> rb{ 1 };

    const auto iters = 100;

    std::vector<T> produced;
    std::vector<T> consumed;

    auto produce = [&]() {
        produced.reserve(iters);
        for (auto i = 0; i < iters; ++i)
        {
            while (!rb.enque(i))
                std::this_thread::yield();
            produced.push_back(i);
        }
    };
    auto consume = [&]() {
        consumed.reserve(iters);
        for (auto i = 0; i < iters; ++i)
        {
            T out;
            while (!rb.deque(out))
                std::this_thread::yield();
            consumed.push_back(out);
        }
    };

    std::thread producer{ produce };
    std::thread consumer{ consume };

    producer.join();
    consumer.join();

    ASSERT_EQ(std::size(produced), std::size(consumed));
    for (auto i = 0; i < iters; ++i)
        ASSERT_EQ(produced[i], consumed[i]) << "error at iteration " << i;
}

GTEST_TEST(RingBuffer, MultiThreadOps)
{
    using T = int;
    RingBuffer<T> rb{ 100 };

    const auto iters = 100'000;

    auto produce = [&]() {
        for (auto i = 0; i < iters; ++i)
            while (!rb.enque(i)) {}
    };
    auto consume = [&]() {
        for (auto i = 0; i < iters; ++i)
        {
            T out;
            while (!rb.deque(out)) {}

            ASSERT_EQ(out, i);
        }
    };

    std::thread producer{ produce };
    std::thread consumer{ consume };

    producer.join();
    consumer.join();
}