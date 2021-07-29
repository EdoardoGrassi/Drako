#include "drako/concurrency/lockfree_mrmw_queue.hpp"

#include <gtest/gtest.h>

#include <thread>

const auto  TEST_THREAD_COUNT = 10;
const auto  TEST_OBJECT_COUNT = 1000 * TEST_THREAD_COUNT;
bool        results[TEST_THREAD_COUNT];
std::thread workers[TEST_THREAD_COUNT];

using namespace drako::lockfree;

void test_task(MR_MW_Queue<int>& q, std::size_t thread_index) noexcept
{
    for (auto i = 0; i < TEST_OBJECT_COUNT; ++i)
    {
        if (int out; !q.deque(out))
            return;
    }
    results[thread_index] = true;
}

GTEST_TEST(MR_MW_Queue, SingleThread)
{
    const auto     capacity = 100;
    MR_MW_Queue<int> queue{ capacity };

    for (auto i = 0; i < capacity; ++i)
        EXPECT_TRUE(queue.enque(i));

    // queue should be full
    EXPECT_FALSE(queue.enque(100));

    for (auto i = 0; i < capacity; ++i)
    {
        int out;
        EXPECT_TRUE(queue.deque(out));
        EXPECT_EQ(out, i);
    }

    // queue should be empty
    int out;
    EXPECT_FALSE(queue.deque(out));
}

GTEST_TEST(MR_MW_Queue, MultiThread)
{
    MR_MW_Queue<int> queue{ TEST_OBJECT_COUNT };

    for (auto i = 0; i < TEST_THREAD_COUNT; ++i)
        workers[i] = std::thread(test_task, std::ref(queue), i);

    for (auto& worker : workers)
        worker.join();

    for (auto result : results)
        EXPECT_TRUE(result);
}