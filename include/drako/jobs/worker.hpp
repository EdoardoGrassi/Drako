#pragma once
#ifndef DRAKO_WORKER_HPP
#define DRAKO_WORKER_HPP

#include "drako/core/lockfree_array_queue.hpp"
#include "drako/core/dk_stack_allocator.hpp"
#include "drako/jobs/job_api.hpp"
#include "drako/jobs/memory_context.hpp"
#include "drako/jobs/job_local_allocator.hpp"

namespace drako::jobs::details
{
    using WorkerHandle = uint32_t;

    // CLASS
    // Executes units of work from a service queue.
    class Worker final
    {
        using local_queue_type = lockfree_array_queue<JobLauncher>;
        using local_allocator_type = job_local_allocator;

    public:

        explicit Worker(size_t stack_size, size_t heap_size) noexcept;
        ~Worker() noexcept;

        Worker(Worker const&) = delete;
        Worker& operator=(Worker const&) = delete;

        // Adds a job_unit to the local work queue.
        bool enqueue(JobLauncher const& job_unit) noexcept
        {
            return _work_queue.enqueue(job_unit);
        }

        // Removes a job_unit from the local work queue.
        bool steal() noexcept;

        template <typename T, typename ...Args>
        DRAKO_NODISCARD local_ptr<T> construct(Args... args)
            noexcept(std::is_nothrow_constructible_v<T, Args...>)
        {
            
        }

    private:

        local_queue_type        _work_queue;
        local_allocator_type    _local_heap;

        void execute() noexcept
        {
            JobLauncher curr;

            while (true)
            {
                if (_work_queue.dequeue(curr))
                {
                    std::invoke(curr.descriptor.call);
                }
                else
                {
                    // try steal
                }
            }
        }
    };
}

#endif // !DRAKO_WORKER_HPP
