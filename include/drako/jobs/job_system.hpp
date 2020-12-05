#pragma once
#ifndef DRAKO_JOB_SYSTEM_HPP
#define DRAKO_JOB_SYSTEM_HPP

//
// @brief   Core class of the concurrent job_unit system.
// @author  Grassi Edoardo
//

#include "drako/concurrency/lockfree_dequeue.hpp"
#include "drako/concurrency/lockfree_pool_allocator.hpp"
#include "drako/concurrency/lockfree_ringbuffer.hpp"
#include "drako/jobs/job_api.hpp"

#include <atomic>
#include <cassert>
#include <thread>

namespace drako::jobs
{
    using WorkerHandle = std::uint32_t;


    /// @brief Executes units of work from a service queue.
    class Worker
    {
    public:
        explicit Worker(std::size_t stack_size, std::size_t heap_size) noexcept;
        ~Worker() noexcept;

        Worker(Worker const&) = delete;
        Worker& operator=(Worker const&) = delete;

        // Removes a job_unit from the local work queue.
        bool steal() noexcept;


    private:
        //local_queue          _work_queue;
        //local_allocator_type _local_heap;
        std::atomic_flag _done;

        void execute() noexcept;
    };

    /// @brief Distributes jobs between a pool of workers.
    class Scheduler
    {
    public:
        explicit Scheduler()
            : _waiters{}, _events{}
        {
            for (auto i = 0; i < std::size(_workers); ++i)
                _workers[i] = std::thread{ _run, std::ref(_queues[i]), std::ref(_done) };
        }

        Scheduler(const Scheduler&) = delete;
        Scheduler& operator=(const Scheduler&) = delete;

        // @brief   Schedules a job_unit.
        //
        void kick(const Job& j) noexcept
        {
            schedule(j);
        }

        // @brief   Schedules a job_unit.
        //
        // Schedules a standalone job_unit, if possible on the hinted worker.
        //
        void kick(const Job& j, WorkerHandle hint) noexcept;

        // Schedules a group of standalone jobs.
        void kick_parallel(const Job jobs[], std::size_t count) noexcept;

        void kick_sequential() noexcept;

        // @brief   Waits for another job_unit to terminate.
        //
        // Suspends executing job_unit until another job_unit completes.
        //
        void wait_for(JobState* context, const JobHandle h) noexcept;

        // @brief   Waits for a job_unit to terminate.
        //
        // Suspends the executing until one of the specified jobs completes.
        //
        void wait_for_any(const JobHandle ctx, const JobHandle jobs[], std::size_t count) noexcept;

        // @brief   Waits for a group of job_unit to terminate.
        //
        // Suspends the executing job_unit until all the specified jobs completes.
        //
        void wait_for_all(const JobHandle ctx, const JobHandle jobs[], std::size_t count) noexcept;



        void submit(const Job& j) noexcept;
        void submit(const Job& j, Event& signal) noexcept;
        void submit(Event& wait, const Job& j) noexcept;
        void submit(Event& wait, const Job& j, Event& signal) noexcept;


        void create_event(std::int32_t init) noexcept;
        void destroy_event() noexcept;

    private:
        struct alignas(std::hardware_destructive_interference_size) _work
        {
            Job    job;
            Event* signal = nullptr;
        };
        //static_assert(sizeof(_work) <= std::hardware_constructive_interference_size,
        //    "Bad class layout: can't fit inside a single cache page.");
        static_assert(sizeof(_work) >= std::hardware_constructive_interference_size,
            "Bad class layout: wasted space inside cache page.");
        static_assert(alignof(_work) >= std::hardware_destructive_interference_size,
            "Bad class layout: cache page alignment not mantained.");

        struct _waiter
        {
            Job                 job;
            std::atomic_int32_t count;
        };
        using _queue = lockfree::SRSWQueue<_work>;

        struct _event
        {
            std::atomic_int32_t counter;
        };

        lockfree::StaticPool<_waiter, 100> _waiters;
        lockfree::StaticPool<Event, 100>   _events;
        std::thread                        _workers[4];
        lockfree::SRSWQueue<_work>         _queues[4];
        std::atomic_flag                   _done;

        void schedule(const Job& j) noexcept;

        _waiter* _make_waiter(const Job& j, std::int32_t counter) noexcept
        {
            using _alty = std::allocator_traits<decltype(_waiters)>;
            auto p      = _alty::allocate(_waiters, 1);
            _alty::construct(_waiters, p, j, counter);
            return p;
        }

        void decrement(_waiter* w) noexcept
        {
            assert(w);
            if (const auto prev = w->count.fetch_sub(1); prev == 1)
            {
                schedule(w->job);
                std::allocator_traits<decltype(_waiters)>::destroy(_waiters, w);
            }
        }

        //void decrement()

        static void _run(_queue& q, std::atomic_flag& done) noexcept
        {
            while (!done.test(std::memory_order::acquire))
            {
                while (const auto item = q.pop())
                {
                    const auto& work = item.value();
                    std::invoke(work.job);

                    if (work.signal)
                    {
                        // TODO: send signal to waiters
                    }
                }
            }
        }
    };


    inline void Scheduler::submit(const Job& j) noexcept
    {
        const _work w{ .job = j };
        const auto  worker = 0;
        assert(_queues[worker].push(w));
    }

    inline void Scheduler::submit(const Job& j, Event& signal) noexcept
    {
        const _work w{ .job = j, .signal = std::addressof(signal) };
        const auto  worker = 0;
        assert(_queues[worker].push(w));
    }

    //inline void Scheduler::submit(Event& wait, const Job& j) noexcept


} // namespace drako::jobs

#endif // !DRAKO_JOB_SYSTEM_HPP