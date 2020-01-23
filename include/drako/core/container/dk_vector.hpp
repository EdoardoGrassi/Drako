#ifndef DRAKO_VECTOR_HPP
#define DRAKO_VECTOR_HPP

#include <cstdlib>
#include <memory>

#include "development/dk_assertion.hpp"

#ifndef DRAKO_DISABLE_CONTAINERS_ASSERTIONS
#define DK_CHECK_INDEX(idx) \
    DRAKO_ASSERT_INSIDE_INCLUSIVE_BOUNDS(idx, 0, size - 1, "Index is out of array bounds")
#else
#   define DK_CHECK_INDEX(...)
#endif

#define CACHE_L1_LINE_SIZE 64
#define GROW_FACTOR 2
#define SHRINK_FACTOR 2

// TODO: add assertions

namespace drako
{
    struct improve_memory_usage {};

    struct improve_access_speed {};

    enum class VectorImpl
    {
        improve_performance,
        improve_memory_usage
    };

    /// <summary>
    /// Dynamic array container.
    /// <para>Not thread safe.</para>
    /// </summary>
    template <typename T,
        typename TImplPolicy,
        typename TAlloc = std::allocator<T> /*, std::size_t GrowFactor, std::size_t ShrinkFactor*/>
        class Vector final
    {
    public:

        struct allocate_exact_size_tag { constexpr explicit allocate_exact_size_tag() = default; };
        struct align_cache_line_tag { constexpr explicit align_cache_line_tag() = default; };

        // Ctor.
        explicit Vector(const size_t count);
        explicit Vector(const size_t count, const allocate_exact_size_tag);

        // Dtor.
        ~Vector() noexcept;

        // Copy ctor.
        Vector(const Vector&);

        // Copy assignment.
        Vector& operator=(const Vector&);

        // Move ctor.
        constexpr Vector(Vector&&) noexcept;

        // Move assignment.
        constexpr Vector& operator=(Vector&&) noexcept;

        constexpr const T& operator[](const size_t idx) const noexcept { return m_data_ptr[idx]; }
        constexpr T& operator[](const size_t idx) noexcept { return m_data_ptr[idx]; }

        // Returns the number of elements stored in the array.
        DRAKO_NODISCARD
            constexpr size_t reserved_memory_size() const noexcept { return size; }

        // Returns the maximum number of elements that can be stored in the array.
        DRAKO_NODISCARD
            constexpr size_t capacity() const noexcept { return m_capacity }

        // Returns a pointer to the first element of the underlying array.
        DRAKO_NODISCARD DRAKO_ALLOCATOR
            constexpr const T* raw_data() const noexcept { return m_data_ptr; }

        DRAKO_NODISCARD DRAKO_ALLOCATOR
            constexpr T* raw_data() noexcept { return m_data_ptr; }

        // Forces a resize of the array so it can contain at least 'count' elements.
        void resize(const size_t count);

        // Inserts an element at the specified index.
        // Relative order of the subsequent elements is preserved.
        void insert_ordered(const size_t idx, const T& element);

        // Inserts an element at the specified index.
        // Relative order of the subsequent elements is not preserved. 
        void insert_unordered(const size_t idx, const T& element);

        // Inserts an element at the end of the array.
        void insert_end(const T& element);

        // Removes the element the specified index.
        // Relative order of the subsequent elements is preserved.
        void remove_ordered(const size_t idx) noexcept;

        // Removes the element the specified index.
        // Relative order of the subsequent elements is not preserved.
        void remove_unordered(const size_t idx) noexcept;

        // Removes the element at the end of the array.
        void remove_end() noexcept;

    private:

        size_t size;
        size_t m_capacity;
        T* m_data_ptr;
    };

    template <typename T>
    class Vector<T, improve_access_speed> {};

    template <typename T>
    class Vector<T, improve_memory_usage> {};

    template <typename T>
    using FastVector = Vector<T, improve_access_speed>;

    template <typename T>
    using CompactVector = Vector<T, improve_memory_usage>;


    template <typename T, typename TImplPolicy, typename TAlloc>
    inline Vector<T, TImplPolicy, TAlloc>::Vector(size_t const count)
        : size(0), m_capacity(GROW_FACTOR * count)
    {
        m_data_ptr = static_cast<T*>(std::aligned_alloc(alignof(T), m_capacity * sizeof(T)));
        DRAKO_ASSERT_NOT_NULL(m_data_ptr, "Array allocation failed");
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline Vector<T, TImplPolicy, TAlloc>::Vector(const size_t count, const allocate_exact_size_tag)
        : size(0), m_capacity(count)
    {
        m_data_ptr = static_cast<T*>(std::aligned_alloc(alignof(T), m_capacity * sizeof(T)));
        DRAKO_ASSERT_NOT_NULL(m_data_ptr, "Array allocation failed");
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline Vector<T, TImplPolicy, TAlloc>::~Vector()
    {
        std::free(m_data_ptr);
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline Vector<T, TImplPolicy, TAlloc>::Vector(const Vector& rhs)
        : size(rhs.size), m_capacity(rhs.m_capacity)
    {
        m_data_ptr = static_cast<T*>(std::aligned_alloc(alignof(T), m_capacity * sizeof(T)));
        DRAKO_ASSERT_NOT_NULL(m_data_ptr, "Array allocation failed");
        std::memcpy(m_data_ptr, rhs.m_data_ptr, size);
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline Vector<T, TImplPolicy, TAlloc>& Vector<T, TImplPolicy, TAlloc>::operator=(const Vector& rhs)
    {
        if ()
        {

        }
        size = rhs.size;
        m_capacity = rhs.m_capacity;

        return *this;
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline constexpr Vector<T, TImplPolicy, TAlloc>::Vector(Vector&& rhs) noexcept
        : size(rhs.size), m_capacity(rhs.m_capacity)
    {
        m_data_ptr = rhs.m_data_ptr;
        rhs.size = 0;
        rhs.m_data_ptr = nullptr;
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline constexpr Vector<T, TImplPolicy, TAlloc>& Vector<T, TImplPolicy, TAlloc>::operator=(Vector&& rhs) noexcept
    {
        DRAKO_ASSERT(this* != rhs, "Move assignment to self is not supported");
        size = rhs.size;
        m_capacity = rhs.m_capacity;
        m_data_ptr = rhs.m_data_ptr;

        rhs.size = 0;
        rhs.m_data_ptr = nullptr;
        return *this;
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline void Vector<T, TImplPolicy, TAlloc>::insert_ordered(const size_t idx, const T& element)
    {
        if (idx == size)
        {
            insert_end(element);
        }
        DK_CHECK_INDEX(idx);
        if (size == m_capacity) // No space left, needs reallocation
        {
            resize(size * GROW_FACTOR);
        }
        // TODO: end implementation
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline void Vector<T, TImplPolicy, TAlloc>::insert_unordered(const size_t idx, const T& element)
    {
        if (idx == size)
        {
            insert_end(element);
        }
        DK_CHECK_INDEX(idx);
        if (size == m_capacity) // No space left, needs reallocation
        {
            resize(size * GROW_FACTOR);
        }
        m_data_ptr[size] = m_data_ptr[idx];
        m_data_ptr[idx] = element;
        size++;
    }

    template <typename T, typename TImplPolicy, typename TAlloc>
    inline void Vector<T, TImplPolicy, TAlloc>::insert_end(const T& element)
    {
        if (size == m_capacity) // No space left, needs reallocation
        {
            resize(size * GROW_FACTOR);
        }
        m_data_ptr[size - 1] = element;
        size++;
    }

    template<typename T, typename TImplPolicy, typename TAlloc>
    inline void Vector<T, TImplPolicy, TAlloc>::remove_ordered(const size_t idx)
    {}

    template<typename T, typename TImplPolicy, typename TAlloc>
    inline void Vector<T, TImplPolicy, TAlloc>::remove_unordered(const size_t idx)
    {}

    template<typename T, typename TImplPolicy, typename TAlloc>
    inline void Vector<T, TImplPolicy, TAlloc>::remove_end()
    {
        if (size > 0)
        {
            if constexpr (std::is_trivially_destructible_v<T>)
            {
                size--;
            }
            else
            {
                size--;
                m_data_ptr[size]->~T(); // Invoke destructor on last element
            }
        }
    }
}

#endif // !DRAKO_VECTOR_HPP
