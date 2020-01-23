#ifndef DRAKO_BPLUSTREE_HPP
#define DRAKO_BPLUSTREE_HPP

#include <thread>

#include "core/dk_compiler_spec.hpp"

#if DRKAPI_DEBUG
#   include "development/dk_assertion.hpp"
#endif

namespace drako
{
    // B+ tree data structure.
    template <typename TKey, typename TVal, typename TAlloc>
    class BPlusTree final
    {
        static_assert(std::is_integral_v<TKey>, STRINGIZE(TKey) + " type must be an integral type");

    public:

        explicit BPlusTree(TAlloc const& allocator) noexcept;
        ~BPlusTree() noexcept;

        BPlusTree(BPlusTree const&) = delete;
        BPlusTree& operator=(BPlusTree& const) = delete;

        BPlusTree(BPlusTree&&) noexcept = delete;
        BPlusTree& operator=(BPlusTree&&) noexcept = delete;

        // Inserts a key-value pair.
        DRAKO_NODISCARD bool insert(TKey const k, TVal const v) noexcept;

        // Removes a key-value pair.
        DRAKO_NODISCARD bool remove(TKey const k) noexcept;

        // Search for a key-value pair.
        DRAKO_NODISCARD bool search(TKey const k, TVal& out) const noexcept;

        // Search for a key-value pair.
        DRAKO_NODISCARD TVal search(TKey const k, TVal const def) const noexcept;

    private:

        static constexpr const size_t cache_line_size = std::hardware_destructive_interference_size;

        struct KeyValuePair
        {
            TKey key;
            TVal val;
        };

        struct KeyNodePair
        {
            TKey    key;
            void*   child;
        };

        struct alignas(cache_line_size) TreeNode
        {
            // TODO: add compiler-time warning when struct doesn't fit in cache.
            static_assert(sizeof(TreeNode) == cache_line_size);

            uint8_t     keys_count = 0;
            KeyNodePair data[branch_factor];

            DRAKO_NODISCARD DRAKO_FORCE_INLINE bool full() const noexcept
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

        void*           _root;          // Ptr to root node
        LeafNode*       _head;          // Ptr to first leaf node
        size_t          _height = 0;    // Distance from root node to leafs
        TAlloc const&   _allocator;     // Memory provider for runtime allocations
    };

    template <typename TKey, typename TVal, typename TAlloc>
    inline BPlusTree<TKey, TVal, TAlloc>::BPlusTree(TAlloc const& allocator) noexcept
        : _allocator(allocator)
    {
        this->_root = this->_allocator.construct();
    }

    template <typename TKey, typename TVal, typename TAlloc>
    inline BPlusTree<TKey, TVal, TAlloc>::~BPlusTree() noexcept
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

    template <typename TKey, typename TVal, typename TAlloc>
    inline bool BPlusTree<TKey, TVal, TAlloc>::insert(const TKey k, const TVal v) noexcept
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
            while (k < curr_node->keys[j]) { j++; }

            curr_node = static_cast<TreeNode*>(curr_node->ptrs[j - 1]);
        }

        // Insert at leaf node
        const auto leaf_node = reinterpret_cast<LeafNode*>(curr_node);
        j = 0;
        while (k < leaf_node->keys[j]) { j++; }

        return true;
    }

    template <typename TKey, typename TVal, typename TAlloc>
    inline bool BPlusTree<TKey, TVal, TAlloc>::search(TKey const k, TVal& out) const noexcept
    {
        // Navigate tree towards leaf nodes
        auto curr_node = static_cast<TreeNode*>(this->_root);
        for (auto i = 0; i < this->_height; i++)
        {
            auto j = 0;
            while (k < curr_node->keys[j]) { j++; }
            curr_node = static_cast<TreeNode*>(curr_node->ptrs[j - 1]);
        }

        // Scan leaf node
        const auto leaf_node = static_cast<LeafNode*>(curr_node);
    }
}

#endif // !DRAKO_BPLUSTREE_HPP
