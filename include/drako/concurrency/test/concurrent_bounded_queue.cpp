#include <thread>

#include "drako/concurrency/lockfree_fixed_queue.hpp"

const auto TEST_THREAD_COUNT = 10;
const auto TEST_OBJECT_COUNT = 1000 * TEST_THREAD_COUNT;
bool results[TEST_THREAD_COUNT];
std::thread workers[TEST_THREAD_COUNT];

void test_task(drako::lockfree_fixed_queue<int>& q, size_t thread_index) noexcept
{
    for (auto i = 0; i < TEST_OBJECT_COUNT; ++i)
    {
        if (auto [state, item] = q.deque(); state != true)
            return;
    }
    results[thread_index] = true;
}

int main()
{
    drako::lockfree_fixed_queue<int> queue{ TEST_OBJECT_COUNT };

    for (auto i = 0; i < TEST_THREAD_COUNT; ++i)
        workers[i] = std::thread(test_task, std::ref(queue), i);

    for (auto& worker : workers)
        worker.join();

    for (auto result : results)
        if (!result)
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}