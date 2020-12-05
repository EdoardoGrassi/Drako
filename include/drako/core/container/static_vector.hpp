#pragma once
#ifndef DRAKO_STATIC_VECTOR_HPP
#define DRAKO_STATIC_VECTOR_HPP

#include <cassert>
#include <cstddef>
#include <memory>

namespace drako
{
    /// @brief Vector type whose max capacity is determined at compile time.
    template <typename T, std::size_t Size>
    class StaticVector
    {
    public:
        using iterator       = T*;
        using const_iterator = T* const;

        constexpr explicit StaticVector() noexcept = default;


        ~StaticVector() noexcept
        {
            for (auto i = 0; i < _size; ++i)
                std::destroy_at(_data + i);
        }

        constexpr StaticVector(const StaticVector& other) requires std::is_copy_constructible_v<T>
        {
            std::destroy_n(std::begin(_data), _size);
            _size = other._size;
            std::copy_n(std::cbegin(other._data), _size, std::begin(_data));
        }

        constexpr StaticVector operator=(const StaticVector& other) requires std::is_copy_constructible_v<T>
        {
            if (this != std::addressof(other)) // prevent self destruction
            {
                std::destroy_n(std::begin(_data), _size);
                _size = other._size;
                std::copy_n(std::cbegin(other._data), _size, std::begin(_data));
            }
        }

        [[nodiscard]] constexpr T& operator[](std::size_t pos) noexcept
        {
            // std::launder is necessary to correctly recover
            // the object previously created with placement new
            return *std::launder(reinterpret_cast<T*>(std::addressof(_data[pos])));
        }

        [[nodiscard]] constexpr const T& operator[](std::size_t pos) const noexcept
        {
            // std::launder is necessary to correctly recover
            // the object previously created with placement new
            return *std::launder(reinterpret_cast<const T*>(std::addressof(_data[pos])));
        }

        [[nodiscard]] constexpr std::size_t size() const noexcept { return _size; }

        [[nodiscard]] constexpr const T* data() const noexcept { return _data; }
        [[nodiscard]] constexpr T*       data() noexcept { return _data; }

        template <typename... Args>
        iterator emplace_back(Args&&... args) requires std::is_constructible_v<T, Args...>
        {
            assert(_size < Size); // out of buffer space
            auto it = std::construct_at(reinterpret_cast<T*>(_data + _size), std::forward<Args>(args)...);
            _size++;
            return it;
        }

        [[nodiscard]] constexpr iterator       begin() { return _data; }
        [[nodiscard]] constexpr const_iterator cbegin() const { return _data; }

        [[nodiscard]] constexpr iterator       end() { return _data + _size; }
        [[nodiscard]] constexpr const_iterator cend() const { return _data + _size; }

    private:
        std::aligned_storage_t<sizeof(T), alignof(T)> _data[Size];
        std::size_t                                   _size;
    };

} // namespace drako

#endif // !DRAKO_STATIC_VECTOR_HPP