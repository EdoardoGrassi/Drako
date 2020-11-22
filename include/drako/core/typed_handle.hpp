#pragma once
#ifndef DRAKO_TYPED_HANDLE_HPP
#define DRAKO_TYPED_HANDLE_HPP

#include <cstdint>
#include <type_traits>

namespace drako
{
    template <typename T, typename Int>
    requires std::is_unsigned_v<Int> class basic_typed_handle
    {
    public:
        using _this = basic_typed_handle;

        explicit constexpr basic_typed_handle() noexcept = default;

        explicit constexpr basic_typed_handle(const Int key) noexcept
            : _key{ key } {}

        constexpr basic_typed_handle(const _this& other) noexcept = default;
        constexpr _this& operator=(const _this& other) noexcept = default;

        [[nodiscard]] constexpr friend bool operator==(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator!=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator<(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator>(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator<=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator>=(const _this, const _this) noexcept = default;

        //[[nodiscard]] constexpr bool operator==(const basic_typed_handle& rhs) const noexcept { return _key == rhs._key; }
        //[[nodiscard]] constexpr bool operator!=(const basic_typed_handle& rhs) const noexcept { return _key != rhs._key; }
        //[[nodiscard]] constexpr bool operator>(const basic_typed_handle& rhs) const noexcept { return _key > rhs._key; }
        //[[nodiscard]] constexpr bool operator<(const basic_typed_handle& rhs) const noexcept { return _key < rhs._key; }
        //[[nodiscard]] constexpr bool operator>=(const basic_typed_handle& rhs) const noexcept { return _key >= rhs._key; }
        //[[nodiscard]] constexpr bool operator<=(const basic_typed_handle& rhs) const noexcept { return _key <= rhs._key; }

        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; }

    private:
        Int _key = 0;
    };

    template <typename T>
    using handle = basic_typed_handle<T, std::uint32_t>;

#define DRAKO_DEFINE_TYPED_HANDLE(type, representation)          \
    class type : public basic_typed_handle<type, representation> \
    {                                                            \
        using _base = basic_typed_handle<representation>;        \
        using _base::_base;                                      \
    }


    template <typename T, typename Int>
    requires std::is_unsigned_v<Int> class basic_typed_id
    {
    public:
        using _this = basic_typed_id;

        explicit constexpr basic_typed_id() noexcept = default;

        explicit constexpr basic_typed_id(const Int key) noexcept
            : _key{ key } {}

        constexpr basic_typed_id(const _this&) noexcept = default;
        constexpr _this& operator=(const _this&) noexcept = default;

        [[nodiscard]] constexpr friend bool operator==(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator!=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator<(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator>(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator<=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator>=(const _this, const _this) noexcept = default;

        //[[nodiscard]] constexpr bool operator==(const basic_typed_id rhs) const noexcept { return _key == rhs._key; }
        //[[nodiscard]] constexpr bool operator!=(const basic_typed_id rhs) const noexcept { return _key != rhs._key; }
        //[[nodiscard]] constexpr bool operator>(const basic_typed_id rhs) const noexcept { return _key > rhs._key; }
        //[[nodiscard]] constexpr bool operator<(const basic_typed_id rhs) const noexcept { return _key < rhs._key; }
        //[[nodiscard]] constexpr bool operator>=(const basic_typed_id rhs) const noexcept { return _key >= rhs._key; }
        //[[nodiscard]] constexpr bool operator<=(const basic_typed_id rhs) const noexcept { return _key <= rhs._key; }

        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; }

    private:
        Int _key = 0;
    };

    template <typename T>
    using id = basic_typed_id<T, std::uint32_t>;


    template <typename Int>
    requires std::is_unsigned_v<Int> class basic_untyped_id
    {
    public:
        using _this = basic_untyped_id;

        explicit constexpr basic_untyped_id() noexcept = default;

        explicit constexpr basic_untyped_id(const Int key) noexcept
            : _key{ key } {}

        constexpr basic_untyped_id(const _this&) noexcept = default;
        constexpr _this& operator=(const _this&) noexcept = default;

        [[nodiscard]] constexpr friend bool operator==(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator!=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator<(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator>(const _this, const _this) noexcept  = default;
        [[nodiscard]] constexpr friend bool operator<=(const _this, const _this) noexcept = default;
        [[nodiscard]] constexpr friend bool operator>=(const _this, const _this) noexcept = default;

        //[[nodiscard]] constexpr bool operator==(const basic_typed_id rhs) const noexcept { return _key == rhs._key; }
        //[[nodiscard]] constexpr bool operator!=(const basic_typed_id rhs) const noexcept { return _key != rhs._key; }
        //[[nodiscard]] constexpr bool operator>(const basic_typed_id rhs) const noexcept { return _key > rhs._key; }
        //[[nodiscard]] constexpr bool operator<(const basic_typed_id rhs) const noexcept { return _key < rhs._key; }
        //[[nodiscard]] constexpr bool operator>=(const basic_typed_id rhs) const noexcept { return _key >= rhs._key; }
        //[[nodiscard]] constexpr bool operator<=(const basic_typed_id rhs) const noexcept { return _key <= rhs._key; }

        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; }

    private:
        Int _key = 0;
    };

    /// @brief Defines a typed id with type T and backing storage S.
#define DRAKO_DEFINE_TYPED_ID(type, storage)      \
    class type : public basic_untyped_id<storage> \
    {                                             \
        using _base = basic_untyped_id<storage>;  \
        using _base::_base;                       \
    }

} // namespace drako

#endif // !DRAKO_TYPED_HANDLE_HPP