//
// \author Grassi Edoardo
//

#ifndef DRAKO_JOBS_DEPENDENCY_MANAGER_HPP
#define DRAKO_JOBS_DEPENDENCY_MANAGER_HPP

#include <atomic>

#include "drako/job_system/job_api.hpp"

#include "drako/core/drako_compiler.hpp"
#include "drako/core/lockfree_pool_allocator.hpp"

namespace drako::jobs::impl
{
    // Represent a logical dependency between jobs.
    struct job_dependency final
    {
        std::atomic<int32_t> _counter;
        const JobLauncher _job;
    };


    typedef uintptr_t dependency_handle;

    // Manages dependecies between jobs.
    template <typename TAllocator = memory::lockfree::object_pool<job_dependency>>
    class dependency_manager final
    {
        using local_allocator_type = TAllocator;

    public:

        // Creates a new dependency.
        DRAKO_NODISCARD const dependency_handle create_dependency(const job_handle with) noexcept;


        // Updates the state of a dependency.
        void update_dependency(const dependency_handle) noexcept;


        // Deletes an existing dependency.
        void remove_dependency(const dependency_handle) noexcept;

    private:

        local_allocator_type _allocator;
    };
}

#endif // !DRAKO_JOBS_DEPENDENCY_MANAGER_HPP
