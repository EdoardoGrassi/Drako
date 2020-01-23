#pragma once
#ifndef DRAKO_JOB_SYSTEM_HPP
#define DRAKO_JOB_SYSTEM_HPP

//
// @brief   Core class of the concurrent job_unit system.
// @author  Grassi Edoardo
//

#include "drako/core/lockfree_pool_allocator.hpp"
#include "drako/core/preprocessor/compiler_macros.hpp"

#include "drako/jobs/job_api.hpp"
#include "drako/jobs/worker.hpp"
#include "drako/jobs/dependency_manager.hpp"

#define HW_THREAD_COUNT 8

namespace drako
{
    // Distributes jobs between a pool of workers.
    //
    class job_scheduler final
    {
        template <typename T>
        using pool = drako::lockfree::object_pool<T>;

    public:

        // @brief   Schedules a job_unit.
        //
        void kick(const job_unit job_unit) noexcept
        {
            job_state_desc jd = _allocator.construct();

            // TODO: equally distribute workload on all workers
            schedule(job_unit);
        }

        // @brief   Schedules a job_unit.
        //
        // Schedules a standalone job_unit, if possible on the hinted worker.
        //
        void kick(job_unit const job_unit, WorkerHandle const hint) noexcept
        {
            JobLauncher new_job(job_unit);
            // First try on hinted thread
            if (this->_workers[10].enqueue(new_job))
            {
                return;
            }
            schedule(job_unit);
        }

        // Schedules a group of standalone jobs.
        void kick_parallel(job_unit const* jobs, size_t const count) noexcept
        {
            for (auto i = 0; i < count; i++)
            {
                _workers[0].enqueue(JobLauncher(jobs[i]));
            }
        }

        void kick_sequential() noexcept;

        // @brief   Waits for another job_unit to terminate.
        //
        // Suspends executing job_unit until another job_unit completes.
        //
        void wait_for(job_state_desc* context, const job_handle handle) noexcept
        {
            if (context->wait_counter.load(std::memory_order_seq_cst) == 0)
            {
                return;
            }
            else
            {
                // Context switch
            }
        }

        // @brief   Waits for a job_unit to terminate.
        //
        // Suspends the executing until one of the specified jobs completes.
        //
        void wait_for_any(const job_handle context, const job_handle jobs[], const size_t count) noexcept
        {

        }

        // @brief   Waits for a group of job_unit to terminate.
        //
        // Suspends the executing job_unit until all the specified jobs completes.
        //
        void wait_for_all(const job_handle context, const job_handle jobs[], const size_t count) noexcept
        {
            // if all the jobs finished return, otherwise move current job_unit to wait list
        }

    private:

        details::Worker         _workers[HW_THREAD_COUNT];
        pool<job_state_desc>    _allocator;

        void schedule(const job_unit job_unit) noexcept;

        void create_dependencies(const job_unit job_unit) noexcept;
    };
}

#endif // !DRAKO_JOB_SYSTEM_HPP
