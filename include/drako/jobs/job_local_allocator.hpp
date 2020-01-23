#pragma once
#ifndef DRAKO_JOBS_LOCAL_ALLOCATOR_HPP
#define DRAKO_JOBS_LOCAL_ALLOCATOR_HPP

#include "drako/core/dk_linear_allocator.hpp"
#include "drako/jobs/memory_context.hpp"

namespace drako::jobs
{
    class job_local_allocator final
    {
        using allocator_type = drako::memory::linear_allocator;
        using marker_type = drako::memory::memory_marker;

    public:

        explicit constexpr job_local_allocator(allocator_type& allocator) noexcept;
        ~job_local_allocator() noexcept
        {
            _allocator.set_rollback_marker(_marker);
        }

        void* allocate(size_t size) noexcept
        {
            return _allocator.allocate(size);
        }

        template <typename T>
        local_ptr<T> allocate() noexcept
        {
            return static_cast<T*>(allocate(sizeof(T));
        }

    private:

        allocator_type& _allocator;
        marker_type     _marker;
    };
}

#endif // !DRAKO_JOBS_LOCAL_ALLOCATOR_HPP
