#pragma once
#ifndef DRAKO_FREE_LIST_HPP
#define DRAKO_FREE_LIST_HPP

#include <memory>

namespace drako
{
    template <typename T, typename Al = std::allocator<T>>
    class FreeList
    {
    public:
        explicit FreeList(const std::size_t size) noexcept
            : _pool(static_cast<list_block*>(std::malloc(sizeof(list_block) * count)))
            , _tail(_pool)
        {
            if (_pool == nullptr)
            {
                std::exit(EXIT_FAILURE);
            }

            for (auto i = 0; i < count - 1; i++)
            {
                _pool[i].next_ptr = _pool + 1;
            }
            _pool[count - 1].next_ptr = nullptr;
        }

        ~FreeList() noexcept
        {
            if (this->_pool != nullptr)
            {
                std::free(this->_pool);
            }
        }

        FreeList(const FreeList&) = delete;
        FreeList& operator=(const FreeList&) = delete;

        constexpr FreeList(FreeList&& other) noexcept
            : _pool(other._pool), _tail(other._tail)
        {
            other._pool = nullptr;
        }

        constexpr FreeList& operator=(FreeList&& other) noexcept
        {
            _pool       = other._pool;
            _tail       = other._tail;
            other._pool = nullptr;
        }

        bool push_back(T&& obj) noexcept;

        bool pop_back(T& out) noexcept;

        void erase(T& obj) noexcept;

    private:
        union list_block
        {
            list_block* next_ptr;
            T           data;
        };

        Al          _al;
        list_block* _pool;
        list_block* _tail;
    };


    // CLASS TEMPLATE
    // Free list with bounded capacity.
    template <typename T, typename Al = std::allocator<T>>
    class FixedFreeList
    {
    public:
    private:
    };

} // namespace drako

#endif // !DRAKO_FREE_LIST_HPP