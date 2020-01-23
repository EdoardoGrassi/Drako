//
// \author  Grassi Edoardo
//

#ifndef DRAKO_LOCKFREE_LINKED_STACK_HPP
#define DRAKO_LOCKFREE_LINKED_STACK_HPP

#include <atomic>

#include "drako/core/drako_compiler.hpp"
#include "drako/core/lockfree_pool_allocator.hpp"

namespace drako::lockfree
{
    template <typename Ty>
    class linked_stack final
    {
    public:

        explicit linked_stack(const size_t size) noexcept : allocator(size) {}
        ~linked_stack() noexcept = default;

        linked_stack(linked_stack const&) = delete;
        linked_stack& operator=(linked_stack const&) = delete;

        linked_stack(linked_stack &&) = delete;
        linked_stack& operator=(linked_stack &&) = delete;


        DRAKO_NODISCARD bool is_empty() const noexcept;

        bool push(Ty const& data) noexcept;

        bool pop(Ty& result) noexcept;

        bool push_unsafe(Ty const& data) noexcept;

        bool peek_unsafe(Ty& result) noexcept;

        bool pop_unsafe(Ty& result) noexcept;

    private:

        struct node final
        {
            const Ty data;
            node* next;

            explicit node(Ty const& data_) noexcept : data(data_) {}
        };

        std::atomic<Ty*> head = nullptr;
        memory::lockfree_object_pool<node> allocator;
    };

    template<typename Ty>
    inline bool linked_stack<Ty>::is_empty() const noexcept
    {
        return this->head.load(std::memory_order_relaxed) == nullptr;
    }

    template<typename Ty>
    inline bool linked_stack<Ty>::push(Ty const& data) noexcept
    {
        node* new_node = new (this->allocator.allocate(sizeof(Ty))) node(data);
        if (new_node == nullptr) { return false; }

        new_node.next = this->head.load();
        while (!this->head.compare_exchange_weak(new_node.next, new_node));
        return true;
    }

    template<typename Ty>
    inline bool linked_stack<Ty>::pop(Ty& result) noexcept
    {
        node* old_head = this->head.load();
        do
        {
            if (old_head == nullptr) { return false; }
        }
        while (!head.compare_exchange_weak(old_head, old_head->next));

        result = old_head;
        this->allocator.deallocate(old_head);
        return true;
    }

    template<typename Ty>
    inline bool linked_stack<Ty>::push_unsafe(Ty const& data) noexcept
    {
        node* new_node = new (this->allocator.allocate(sizeof(Ty))) node(data);
        if (new_node == nullptr) { return false; }

        new_node.next = this->head.load(std::memory_order_relaxed);
        this->head.store(new_node, std::memory_order_relaxed);
        return true;
    }

    template<typename Ty>
    inline bool linked_stack<Ty>::peek_unsafe(Ty& result) noexcept
    {
        auto curr_head = this->head.load(std::memory_order_relaxed);
        if (curr_head == nullptr) { return false; }
        result = curr_head;
        return true;
    }

    template<typename Ty>
    inline bool linked_stack<Ty>::pop_unsafe(Ty& result) noexcept
    {
        auto curr_head = this->head.load(std::memory_order_relaxed);
        if (curr_head == nullptr) { return false; }
        this->head = curr_head.next;
        result = curr_head;
        return true;
    }
}

#endif // !DRAKO_LOCKFREE_LINKED_STACK_HPP
