#pragma once
#ifndef DRAKO_SYNC_MEMORY_POOL_HPP
#define DRAKO_SYNC_MEMORY_POOL_HPP

#include <atomic>

namespace drako
{
    // CLASS TEMPLATE
    // Thread-safe memory pool.
    template <size_t Size, size_t Align>
    class sync_memory_pool
    {
    public:

        explicit constexpr sync_memory_pool(size_t capacity) noexcept
        {

        }

    private:

        union pool_node
        {

        };


    };

} // namespace drako

#endif // !DRAKO_SYNC_MEMORY_POOL_HPP
