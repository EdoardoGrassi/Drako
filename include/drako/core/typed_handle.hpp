#pragma once
#ifndef DRAKO_TYPED_HANDLE_HPP
#define DRAKO_TYPED_HANDLE_HPP

#include <cstdint>
#include <type_traits>

namespace drako
{
    template <typename T, typename Int>
    requires std::is_unsigned_v<Int> class BasicTypedHandle
    {
    public:
        explicit constexpr BasicTypedHandle() noexcept = default;

        explicit constexpr BasicTypedHandle(const Int key) noexcept
            : _key{ key } {}

        constexpr BasicTypedHandle(const BasicTypedHandle&) noexcept = default;
        constexpr BasicTypedHandle& operator=(const BasicTypedHandle&) noexcept = default;

        //[[nodiscard]] friend bool operator==(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator!=(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator<(const _this, const _this) noexcept  = default;
        //[[nodiscard]] friend bool operator>(const _this, const _this) noexcept  = default;
        //[[nodiscard]] friend bool operator<=(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator>=(const _this, const _this) noexcept = default;

        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; }

    private:
        Int _key = 0;
    };


    /// @brief Defines a strongly typed handle with name Type and storage Integer.
#define DRAKO_DEFINE_TYPED_HANDLE(Type, Integer)                 \
    class Type : public BasicTypedHandle<Type, Integer>          \
    {                                                            \
        using BasicTypedHandle<Type, Integer>::BasicTypedHandle; \
    };                                                           \
    static_assert(std::is_nothrow_copy_assignable_v<Type>,       \
        "Generated type doesn't meet Handle requirement.");      \
    static_assert(std::is_nothrow_copy_constructible_v<Type>,    \
        "Generated type doesn't meet Handle requirement.");


    template <typename Int>
    requires std::is_unsigned_v<Int> class BasicTypedID
    {
    public:
        explicit constexpr BasicTypedID() noexcept = default;

        explicit constexpr BasicTypedID(const Int key) noexcept
            : _key{ key } {}

        constexpr BasicTypedID(const BasicTypedID&) noexcept = default;
        constexpr BasicTypedID& operator=(const BasicTypedID&) noexcept = default;

        //[[nodiscard]] friend bool operator==(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator!=(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator<(const _this, const _this) noexcept  = default;
        //[[nodiscard]] friend bool operator>(const _this, const _this) noexcept  = default;
        //[[nodiscard]] friend bool operator<=(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator>=(const _this, const _this) noexcept = default;

        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; }

    private:
        Int _key = 0;
    };


    /// @brief Defines a strongly typed id with name Type and storage Integer.
#define DRAKO_DEFINE_TYPED_ID(Type, Integer)                  \
    class Type : public BasicTypedID<Integer>                 \
    {                                                         \
        using BasicTypedID<Integer>::BasicTypedID;            \
    };                                                        \
    static_assert(std::is_nothrow_copy_assignable_v<Type>,    \
        "Generated type doesn't meet ID requirement.");       \
    static_assert(std::is_nothrow_copy_constructible_v<Type>, \
        "Generated type doesn't meet ID requirement.");


    /*
/// @brief Defines a strongly typed id with name Type and storage Integer.
#define DRAKO_DEFINE_TYPED_ID(Type, Integer)                                         \
    requires std::is_unsigned_v<Integer> class Type                                  \
    {                                                                                \
    public:                                                                          \
        explicit constexpr Type() noexcept = default;                                \
        explicit constexpr Type(const Integer key) noexcept : _key{ key } {}         \
                                                                                     \
        constexpr Type(const Type&) noexcept = default;                              \
        constexpr Type& operator=(const Type&) noexcept = default;                   \
                                                                                     \
        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; } \
                                                                                     \
    private:                                                                         \
        Integer _key = 0;                                                            \
    };                                                                               \
    static_assert(std::is_nothrow_copy_assignable_v<Type>,                           \
        "Generated type doesn't meet ID requirement.");                              \
    static_assert(std::is_nothrow_copy_constructible_v<Type>,                        \
        "Generated type doesn't meet ID requirement.");*/


    template <typename Int>
    requires std::is_unsigned_v<Int> class BasicUntypedID
    {
    public:
        explicit constexpr BasicUntypedID() noexcept = default;

        explicit constexpr BasicUntypedID(const Int key) noexcept
            : _key{ key } {}

        constexpr BasicUntypedID(const BasicUntypedID&) noexcept = default;
        constexpr BasicUntypedID& operator=(const BasicUntypedID&) noexcept = default;

        //[[nodiscard]] friend bool operator==(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator!=(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator<(const _this, const _this) noexcept  = default;
        //[[nodiscard]] friend bool operator>(const _this, const _this) noexcept  = default;
        //[[nodiscard]] friend bool operator<=(const _this, const _this) noexcept = default;
        //[[nodiscard]] friend bool operator>=(const _this, const _this) noexcept = default;

        [[nodiscard]] constexpr operator bool() const noexcept { return _key != 0; }

    private:
        Int _key = 0;
    };

    /// @brief Defines a weakly typed id with name T and storage S.
#define DRAKO_DEFINE_UNTYPED_ID(type, representation)         \
    class type : public BasicUntypedID<representation>        \
    {                                                         \
        using BasicUntypedID::BasicUntypedID                  \
    };                                                        \
    static_assert(std::is_nothrow_copy_assignable_v<type>,    \
        "Generated type doesn't meet ID requirement.");       \
    static_assert(std::is_nothrow_copy_constructible_v<type>, \
        "Generated type doesn't meet ID requirement.");

} // namespace drako

#endif // !DRAKO_TYPED_HANDLE_HPP