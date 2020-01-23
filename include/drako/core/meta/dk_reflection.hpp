#pragma once
#ifndef DRAKO_REFLECTION_HPP
#define DRAKO_REFLECTION_HPP

#include <string_view>
#include <vector>

#include "drako/core/preprocessor/compiler_macros.hpp"

namespace drako::meta
{
    // Stores metadata about a type.
    //
    template <typename T>
    class type_info
    {
    };

    // Stores metadata about a field.
    //
    template <typename T, typename TField, const char* FieldName>
    struct field_info
    {
    };

    // Stores metadata about a method.
    //
    template <typename T, typename TFunction, const char* MethodName>
    struct method_info
    {
    };


    // Reflects the type.
    template <typename T>
    struct reflect_type
    {
        template <typename TField, const char* FieldName>
        using reflect_field = field_info<T, TField, FieldName>;

        template <typename TFunction, const char* MethodName>
        using reflect_method = method_info<T, TFunction, MethodName>;

        template <typename TField, const char* FieldName>
        static constexpr field_info<T, TField, FieldName> get_field_descriptor() noexcept;
    };


    #if !defined(DRAKO_API_NOREFLECTION)

    // MACRO: Local type name.
    #define DRAKO_META_IMPL_TYPENAME DrakoReflectionLocalType

    // MACRO: Enables reflection for the type.
    // Must be placed inside type declaration before any other reflection macros.
    #define DRAKO_META_TYPE(Type) using DRAKO_META_IMPL_TYPENAME = reflect_type<Type>; // Parenthesis around type aren't allowed

    /*
    template <> \
    friend static constexpr auto reflect_type<DRAKO_REFLECTED_TYPE>::get_type_descriptor() \
    { \
        return drako::meta::TypeDescriptor<DRAKO_REFLECTED_TYPE>(); \
    }*/


    // MACRO: Exposes a member field to reflection functions. Must be placed inside type declaration.</summary>
    #define DRAKO_META_FIELD(Field) \
    template <> \
    friend static constexpr auto reflect_type<DRAKO_META_IMPL_TYPENAME>::get_field_descriptor<decltype(Field), #Field>() noexcept \
    { \
        return drako::meta::FieldDescriptor(&DRAKO_META_IMPL_TYPENAME::Field); \
    }


    /// <summary>Exposes a member method to reflection functions. Must be placed inside type declaration.</summary>
    #define DRAKO_META_METHOD(Method)

    #define DRAKO_SERIALIZE_FIELD(Field) \
    using FieldDescriptor<__COUNTER__> = drako::meta::FieldInfo<DRAKO_META_IMPL_TYPENAME, decltype(Field), &DRAKO_META_IMPL_TYPENAME::Field>;


    #else

    #define DRAKO_META_TYPE(Type)
    #define DRAKO_META_FIELD(Field)
    #define DRAKO_META_METHOD(Method)

    #endif
}

#endif // !DRAKO_REFLECTION_HPP