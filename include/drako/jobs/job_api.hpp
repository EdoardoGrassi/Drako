#pragma once
#ifndef DRAKO_JOBS_API_HPP
#define DRAKO_JOBS_API_HPP

#include <any>
#include <atomic>
#include <cstdint>
#include <thread>

#define DRAKO_JOB [[drako::job_declaration]]

namespace drako
{
    // CONSTANT
    // Byte size of a L1 cache page.
    static constexpr const auto L1_PAGE_SIZE = std::hardware_constructive_interference_size;

    struct job_args final
    {
        void* packed_args;
    };

    // Signature of a function that can be schedule for execution in the job_unit system.
    using job_routine   = void(*)(const job_args);
    using job_priority  = uint32_t;
    using job_handle    = uintptr_t;


    // STRUCTURE
    // Standalone unit of work that can be scheduled for execution.
    class alignas(L1_PAGE_SIZE) job_unit final
    {
    public:

        template <typename ...Args, void(JobFunction)(Args...)>
        explicit constexpr job_unit(Args... args) noexcept
        {
            static_assert(sizeof(Args...) <= sizeof(_args), "Not enough space to embed arguments");

            _args = std::make_tuple(args);
            _call = &job_call_wrapper<Args..., JobFunction>;
        }

        constexpr bool is_runnable() const noexcept
        {
            return true;
        }

    private:

        uint8_t         _args[L1_PAGE_SIZE - (sizeof(job_routine) + sizeof(job_chain_desc*))];
        job_chain_desc* _dependencies;
        job_routine     _call;

        // Unified wrapper for scheduled job_unit function.
        //
        template <typename ...Args, void(JobFunction)(Args...)>
        static void job_call_wrapper(const job_args args) noexcept
        {
            auto unpacked_args = static_cast<std::tuple<Args...>>(args.packed_args);
            std::apply(JobFunction, unpacked_args);
        }
    };
    static_assert(sizeof(job_unit) == L1_PAGE_SIZE, "Cache alignment not mantained");


    // Runtime state of a job_unit instance.
    //
    struct job_state_desc final
    {
        std::atomic<uint32_t> wait_counter;
    };


    // Runtime scheduler dependencies of a job_unit instance.
    //
    class job_chain_desc final
    {
        explicit constexpr job_chain_desc(uint32_t counter) noexcept
            : _counter(counter), _dependencies()
        {
        }

        constexpr job_chain_desc(const job_chain_desc&) noexcept;
        constexpr job_chain_desc& operator=(const job_chain_desc&) noexcept;

        void set_dependency(const job_handle h) noexcept;

    private:

        std::atomic<uint32_t>   _counter = 0;
        job_handle              _dependencies[8];
    };


    // Group of jobs that shares dependencies.
    //
    struct job_batch final
    {

    };


    // Synchronizes the execution of different jobs.
    //
    struct job_scheduler_fence final
    {
        std::atomic<int32_t>    counter;
        JobWaiter* waiters[];

        explicit constexpr job_scheduler_fence() noexcept;
    };
}

#endif // !DRAKO_JOBS_API_HPP

