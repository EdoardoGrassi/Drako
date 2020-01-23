#ifndef DRAKO_HASHED_ARRAY_TREE_HPP_
#define DRAKO_HASHED_ARRAY_TREE_HPP_

#include <cstddef>
#include <cmath>

#include <drako_math_utility.hpp>
#include <drako_assertions.hpp>

#ifndef DRAKO_DISABLE_BOUNDS_CHECKS
#   define DK_CHECK_INDEX(index) \
        DRAKO_ASSERT_INSIDE_INCLUSIVE_BOUNDS(index, 0, capacity() - 1, "Index is out of array bounds")
#else
#   define DK_ASSERT_BOUNDS(...)
#endif

namespace drako
{
    #define CACHE_LINE_SIZE_BYTES 64

    // Tag structure
    struct request_contiguous_storage_tag
    {
        constexpr explicit request_contiguous_storage_tag() noexcept = default;
    };

    template <typename T, std::size_t ChunkSize>
    class HashedArrayTree
    {
        static_assert(drako::math::is_pow2(ChunkSize), "Chunk size must be a power of 2");

    public:

        // Ctor
        explicit HashedArrayTree(std::size_t count);
        // Ctor
        explicit HashedArrayTree(std::size_t count, const request_contiguous_storage_tag);
        // Dtor
        ~HashedArrayTree() noexcept;

        // Copy ctor
        HashedArrayTree(const HashedArrayTree&);
        // Copy assignment operator
        HashedArrayTree& operator=(const HashedArrayTree&);

        // Move ctor
        constexpr HashedArrayTree(HashedArrayTree&&) noexcept;
        // Move assignment operator
        constexpr HashedArrayTree& operator=(HashedArrayTree&&) noexcept;

        inline constexpr T& operator[](std::size_t idx) noexcept;
        inline constexpr const T& operator[](std::size_t idx) const noexcept;

        // Returns the number of elements stored in the array
        inline constexpr std::size_t reserved_memory_size() const noexcept { return size; }

        // Returns the maximum number of elements that can be stored in the array
        inline constexpr std::size_t capacity() const noexcept { return m_chunkCount * ChunkSize; }

    private:

        std::size_t size;
        std::size_t m_chunkCount;
        T** m_pChunks;
    };

    template<typename T, std::size_t ChunkSize>
    HashedArrayTree<T, ChunkSize>::HashedArrayTree(std::size_t count)
        : size(0)
    {
        m_chunkCount = drako::math::ceil_to_pow2(std::sqrt(count));

        T** m_pChunks = static_cast<T**>(std::aligned_alloc(alignof(T*), m_chunkCount * sizeof(T*));
        DRAKO_ASSERT_NOT_NULL(m_pChunks, "Allocation failed");

        for (std::size_t i = 0; i < m_chunkCount; i++)
        {
            m_pChunks[i] = static_cast<T*>(std::aligned_alloc(alignof(T), ChunkSize * sizeof(T)));
            DRAKO_ASSERT_NOT_NULL(m_pChunks[i], "Allocation failed");
        }
    }

    template<typename T, std::size_t ChunkSize>
    HashedArrayTree<T, ChunkSize>::HashedArrayTree(std::size_t count, const request_contiguous_storage_tag)
        : size(0)
    {
        m_chunkCount = drako::math::ceil_to_pow2(std::sqrt(count));

        T** m_pChunks = static_cast<T**>(
            std::aligned_alloc(alignof(T),
                               m_chunkCount * sizeof(T*) + m_chunkCount * ChunkSize * sizeof(T))
            );
        DRAKO_ASSERT_NOT_NULL(m_pChunks, "Allocation failed");

        for (std::size_t i = 0; i < m_chunkCount; i++)
        {
            m_pChunks[i] = // assign leafs
        }
    }

    template<typename T, std::size_t ChunkSize>
    inline HashedArrayTree<T, ChunkSize>::~HashedArrayTree() noexcept
    {
        if (m_pChunks != nullptr) // Avoid inner ptrs deallocations if the array has been moved from
        {
            for (int i = 0; i < m_chunkCount; i++)
            {
                std::free(m_pChunks[i]);
            }
            std::free(m_pChunks);
        }
    }

    template<typename T, std::size_t ChunkSize>
    inline HashedArrayTree<T, ChunkSize>::HashedArrayTree(const HashedArrayTree& rhs)
        : size(rhs.size), m_chunkCount(rhs.m_chunkCount)
    {
        
    }

    template<typename T, std::size_t ChunkSize>
    inline HashedArrayTree<T, ChunkSize>& HashedArrayTree<T, ChunkSize>::operator=(const HashedArrayTree& rhs)
    {
        if (this != &rhs)
        {
            // TODO: add impl
        }
        return *this;
    }

    template<typename T, std::size_t ChunkSize>
    inline constexpr HashedArrayTree<T, ChunkSize>::HashedArrayTree(HashedArrayTree&& rhs) noexcept
        : size(rhs.size), m_chunkCount(rhs.m_chunkCount)
    {
        m_pChunks = rhs.m_pChunks;
        rhs.m_pChunks = nullptr;
    }

    template<typename T, std::size_t ChunkSize>
    inline constexpr HashedArrayTree<T, ChunkSize>& HashedArrayTree<T, ChunkSize>::operator=(HashedArrayTree&& rhs) noexcept
    {
        if (this != &rhs)
        {
            // TODO: add impl
        }
        return *this;
    }

    template <typename T, std::size_t ChunkSize>
    inline constexpr T& HashedArrayTree<T, ChunkSize>::operator[](std::size_t idx) noexcept
    {
        DK_CHECK_INDEX(idx);
        return (m_pChunks[idx / ChunkSize])[idx % ChunkSize];
    }

    template <typename T, std::size_t ChunkSize>
    inline constexpr const T& HashedArrayTree<T, ChunkSize>::operator[](std::size_t idx) const noexcept
    {
        DK_CHECK_INDEX(idx);
        return (m_pChunks[idx / ChunkSize])[idx % ChunckSize];
    }
}

#endif // !DRAKO_HASHED_ARRAY_TREE_HPP_
