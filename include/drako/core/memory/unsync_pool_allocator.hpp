#pragma once
#ifndef DRAKO_UNSYNC_POOL_ALLOCATOR_HPP
#define DRAKO_UNSYNC_POOL_ALLOCATOR_HPP

/// @file
///
/// @brief      Unsynchronized pool allocator.
/// @author     Grassi Edoardo

#include <cassert>
#include <memory>

namespace drako
{
    /// @brief Pool allocator with static backing memory.
    template <typename T, std::size_t Size>
    class StaticPool
    {
    public:
        explicit StaticPool() noexcept;

        StaticPool(const StaticPool&) = delete;
        StaticPool& operator=(const StaticPool&) = delete;

        StaticPool(StaticPool&&) noexcept = delete;
        StaticPool& operator=(StaticPool&&) noexcept = delete;

    private:
        union _block
        {
            std::aligned_storage_t<sizeof(T), alignof(T)> storage;
            _block*                                       next;
        };

        _block* _free;
        _block  _pool[Size];
    };

    /// @brief Pool allocator with runtime allocated backing memory.
    template <typename T>
    class Pool
    {
    public:
        /// @brief Constructs a pool with given capacity.
        ///
        /// @param[in] capacity Number of object that can be stored.
        ///
        explicit Pool(std::size_t capacity)
            : _pool{ std::make_unique<_block[]>(capacity) }
        {
            assert(capacity > 0);

            // link blocks as a free list
            _free = _pool.get();
            for (auto i = 0; i < capacity - 1; ++i)
                _pool[i].next = _pool.get() + i + 1;
            _pool[capacity - 1].next = nullptr; // last block with no successor
        }

        Pool(const Pool&) = delete;
        Pool& operator=(const Pool&) = delete;

        Pool(Pool&&) noexcept = delete;
        Pool& operator=(Pool&&) noexcept = delete;


        /// @brief Allocates memory.
        ///
        /// @param[in] n Must be 1. Provided for compatibility with std::allocator.
        ///
        [[nodiscard]] T* allocate(std::size_t n)
        {
            assert(n == 1);
            if (_free == nullptr)
                [[unlikely]] throw std::bad_alloc{ "Out of pool memory." };

            const auto p = _free;
            _free        = _free->next;
            return reinterpret_cast<T*>(std::addressof(p->storage));
        }

        /// @brief Deallocates memory.

        /// @param[in] p Previously allocated object.
        /// @param[in] n Must be 1. Provided for compatibility with std::allocator.
        ///
        void deallocate(T* p, std::size_t n) noexcept
        {
            assert(p);
            assert(n == 1);
            auto block = reinterpret_cast<_block*>(p);
            p->next    = _free;
            _free      = p;
        }


        /// @brief Checks if the allocator is full.
        [[nodiscard]] constexpr bool full() const noexcept
        {
            return _free == nullptr;
        }


        /// @brief Converts an object pointer to corresponding index inside the pool.
        [[nodiscard]] constexpr std::size_t ptr_to_index(T* ptr) const noexcept
        {
            return static_cast<std::size_t>(std::distance(_pool.get(), reinterpret_cast<_block*>(ptr)));
        }


        /// @brief Converts an object index to corresponding pointer inside the pool.
        [[nodiscard]] constexpr T* index_to_ptr(std::size_t idx) const noexcept
        {
            return reinterpret_cast<_block*>(std::addressof(_pool[idx].storage));
        }

    private:
        union _block
        {
            std::aligned_storage_t<sizeof(T), alignof(T)> storage;
            _block*                                       next;
        };

        _block*                   _free; // freelist of unallocated blocks
        std::unique_ptr<_block[]> _pool; // backing buffer
        //const std::size_t         _size; // number of preallocated blocks
    };

} // namespace drako

#endif // !DRAKO_UNSYNC_POOL_ALLOCATOR_HPP