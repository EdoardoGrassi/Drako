#pragma once
#ifndef DRAKO_CONCURRENT_LIST_HPP
#define DRAKO_CONCURRENT_LIST_HPP

#include <atomic>

#include "core/compiler_macros.hpp"

namespace drako::concurrency
{

    // CLASS TEMPLATE
    // Thread-safe list implemented with lazy syncronization.
    //
    template <typename T>
    class lazy_list final
    {
    public:

    private:
    };


    // CLASS TEMPLATE
    // Lock-free linearizable list based on Harris-Michael algorithm.
    //
    template <typename T>
    class lockfree_list final
    {
    public:

        explicit lockfree_list() noexcept;
        ~lockfree_list() noexcept;

        lockfree_list(const lockfree_list&) = delete;
        lockfree_list& lockfree_list(const lockfree_list&) = delete;

        void insert() noexcept;

        void discard() noexcept;

        void remove() noexcept;

        DRAKO_NODISCARD
            bool contains(const T& value) noexcept;

    private:

        std::atomic<T*> _head;
        std::atomic<T*> _tail;
    };

} // namespace drako::concurrency

#endif // !DRAKO_CONCURRENT_LIST_HPP
