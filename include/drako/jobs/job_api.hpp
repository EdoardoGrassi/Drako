#pragma once
#ifndef DRAKO_JOBS_API_HPP
#define DRAKO_JOBS_API_HPP

#include <atomic>
#include <cstdint>
#include <functional>
#include <thread>

namespace drako::jobs
{
    struct JobArgs final
    {
        void* packed_args;
    };

    // Signature of a function that can be schedule for execution in the job_unit system.
    using JobRoutine  = void (*)(const JobArgs);
    using JobPriority = uint32_t;
    using JobHandle   = uintptr_t;


    /// @brief Standalone unit of work that can be scheduled.
    using Job = std::function<void()>;

    /*
    class alignas(std::hardware_destructive_interference_size) Job
    {
    public:
        template <typename... Args, void(JobFunction)(Args...)>
        explicit constexpr Job(Args... args) noexcept
        {
            static_assert(sizeof(Args...) <= sizeof(_args) "Not enough space to embed arguments locally.");

            _args = std::make_tuple(args);
            _call = &_wrap<Args..., JobFunction>;
        }

    private:
        std::byte  _args[std::hardware_constructive_interference_size - (sizeof(JobRoutine) + sizeof(JobChain*))];
        JobChain*  _dependencies;
        JobRoutine _call;

        // unified wrapper for scheduled job_unit function.
        template <typename... Args, void(JobFunction)(Args...)>
        static void _wrap(const JobArgs args) noexcept
        {
            auto unpacked_args = static_cast<std::tuple<Args...>>(args.packed_args);
            std::apply(JobFunction, unpacked_args);
        }
    };
    static_assert(sizeof(Job) == std::hardware_constructive_interference_size,
        "Bad class layout: cache page size not mantained.");
    static_assert(alignof(Job) >= std::hardware_destructive_interference_size,
        "Bad class layout: cache page alignment not mantained.");
        */


    // Runtime state of a job_unit instance.
    //
    struct JobState
    {
        std::atomic<uint32_t> wait_counter;
    };


    // Runtime scheduler dependencies of a job_unit instance.
    //
    class JobChain
    {
        explicit constexpr JobChain(uint32_t counter) noexcept
            : _counter(counter), _dependencies()
        {
        }

        constexpr JobChain(const JobChain&) noexcept;
        constexpr JobChain& operator=(const JobChain&) noexcept;

        void set_dependency(const JobHandle h) noexcept;

    private:
        std::atomic<uint32_t> _counter = 0;
        JobHandle             _dependencies[8];
    };


    /// @brief Group of jobs that shares dependencies.
    struct Batch
    {
    };

    struct Waiter
    {
    };

    // Synchronizes the execution of different jobs.
    //
    class Event
    {
        explicit constexpr Event(std::int32_t count) noexcept;

        void signal() noexcept;

    private:
        std::atomic<std::int32_t> _counter;
        Waiter                    _waiters[10];
    };
} // namespace drako::jobs

#endif // !DRAKO_JOBS_API_HPP
