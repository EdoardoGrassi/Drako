#ifndef DRAKO_BPTREE_HPP
#define DRAKO_BPTREE_HPP

#include <thread>

namespace drako
{
    // B+ tree data structure.
    template <typename Key, typename Val, typename Alloc>
    class bptree final
    {
        static_assert(std::is_integral_v<Key>, "Key type must be an integral type");

    public:
        explicit bptree(TAlloc const& allocator) noexcept;
        ~bptree() noexcept;

        bptree(bptree const&) = delete;
        bptree& operator=(bptree& const) = delete;

        bptree(bptree&&) noexcept = delete;
        bptree& operator=(bptree&&) noexcept = delete;

        // Inserts a key-value pair.
        [[nodiscard]] bool insert(TKey const k, TVal const v) noexcept;

        // Removes a key-value pair.
        [[nodiscard]] bool remove(TKey const k) noexcept;

        // Search for a key-value pair.
        [[nodiscard]] bool search(TKey const k, TVal& out) const noexcept;

        // Search for a key-value pair.
        [[nodiscard]] TVal search(TKey const k, TVal const def) const noexcept;

    private:
        constexpr const size_t cache_line_size = std::hardware_destructive_interference_size;

        struct KeyValuePair
        {
            TKey key;
            TVal val;
        };

        struct KeyNodePair
        {
            TKey  key;
            void* child;
        };

        struct alignas(cache_line_size) TreeNode
        {
            // TODO: add compiler-time warning when struct doesn't fit in cache.
            static_assert(sizeof(TreeNode) == cache_line_size);

            uint8_t     keys_count = 0;
            KeyNodePair data[branch_factor];

            [[nodiscard]] inline bool full() const noexcept
            {
                return this->keys_count == keys_count;
            }

        private:
            constexpr const size_t branch_factor = 10;
        };

        struct alignas(cache_line_size) LeafNode
        {
            // TODO: add compiler-time warning when struct doesn't fit in cache.
            static_assert(sizeof(LeafNode) == CACHE_LINE_SIZE);

            KeyValuePair data[branch_factor];
            LeafNode*    next = nullptr;
        };

        void*        _root;       // Ptr to root node
        LeafNode*    _head;       // Ptr to first leaf node
        size_t       _height = 0; // Distance from root node to leafs
        Alloc const& _allocator;  // Memory provider for runtime allocations
    };

    template <typename Key, typename Val, typename Alloc>
    inline bptree<Key, Val, Alloc>::bptree(Alloc const& allocator) noexcept
        : _allocator(allocator)
    {
        _root = _allocator.construct();
    }

    template <typename Key, typename Val, typename Alloc>
    inline bptree<Key, Val, Alloc>::~bptree() noexcept
    {
        auto curr_node = static_cast<TreeNode*>(this->_root);

        // Foreach node
        for (auto i = 0; i < this->_height; i++)
        {
            // Foreach node child
            for (auto j = 0; curr_node->data[j] != nullptr)
            {
                // Recursive call - free child
            }
        }
        this->_allocator.deallocate(this->_root);
    }

    template <typename Key, typename Val, typename Alloc>
    inline bool bptree<Key, Val, Alloc>::insert(const Key k, const Val v) noexcept
    {
        // Navigate tree toward leaf nodes
        auto curr_node = static_cast<TreeNode*>(this->_root);
        for (auto i = 0; i < this->_height; i++)
        {
            // Split current node if already full
            if (curr_node.full())
            {
                // Split node
            }

            // Find correct child node by key comparison
            auto j = 0;
            while (k < curr_node->keys[j])
            {
                j++;
            }

            curr_node = static_cast<TreeNode*>(curr_node->ptrs[j - 1]);
        }

        // Insert at leaf node
        const auto leaf_node = reinterpret_cast<LeafNode*>(curr_node);
        j                    = 0;
        while (k < leaf_node->keys[j])
        {
            j++;
        }

        return true;
    }

    template <typename Key, typename Val, typename Alloc>
    inline bool bptree<Key, Val, Alloc>::search(Key const k, Val& out) const noexcept
    {
        // Navigate tree towards leaf nodes
        auto curr_node = static_cast<TreeNode*>(this->_root);
        for (auto i = 0; i < this->_height; i++)
        {
            auto j = 0;
            while (k < curr_node->keys[j])
            {
                j++;
            }
            curr_node = static_cast<TreeNode*>(curr_node->ptrs[j - 1]);
        }

        // Scan leaf node
        const auto leaf_node = static_cast<LeafNode*>(curr_node);
    }
} // namespace drako

#endif // !DRAKO_BPLUSTREE_HPP
