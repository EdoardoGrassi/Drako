#pragma once
#ifndef DRAKO_FREE_LIST_HPP
#define DRAKO_FREE_LIST_HPP

#include <memory>

#include "development/dk_assertion.hpp"

namespace drako
{
    template <typename T>
    class free_list
    {
    public:

        explicit free_list(const size_t size) noexcept
            : _pool(static_cast<list_block*>(std::malloc(sizeof(list_block)* count)))
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

        ~free_list() noexcept
        {
            if (this->_pool != nullptr)
            {
                std::free(this->_pool);
            }
        }

        free_list(const free_list&) = delete;
        free_list& operator=(const free_list&) = delete;

        constexpr free_list(free_list&& other) noexcept
            : _pool(other._pool), _tail(other._tail)
        {
            other._pool = nullptr;
        }

        constexpr free_list& operator=(free_list&& other) noexcept
        {
            _pool = other._pool;
            _tail = other._tail;
            other._pool = nullptr;
        }

        result_code push_back(T&& obj) noexcept
        {
            if (this->_tail == nullptr)
            {
                return result_code::error_out_of_memory;
            }
            auto old_block = this->_tail;
            this->_tail = this->_tail->next_ptr;
            old_block->data = T(obj);

            return result_code::success;
        }

        result_code pop_back(T& out) noexcept
        {
            if (this->_tail == nullptr)
            {
                return result_code::failure;
            }

            return result_code::success;
        }

        void erase(T& obj) noexcept;

    private:

        union list_block
        {
            list_block* next_ptr;
            T data;
        };

        list_block* _pool;
        list_block* _tail;
    };


    // CLASS TEMPLATE
    // Free list with static memory allocation.
    template <typename T>
    class fixed_free_list
    {
    public:

    private:
    };

} // namespace drako

#endif // !DRAKO_FREE_LIST_HPP

