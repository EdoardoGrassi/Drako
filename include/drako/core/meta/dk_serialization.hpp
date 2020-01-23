#pragma once
#ifndef DRAKO_SERIALIZATION_HPP
#define DRAKO_SERIALIZATION_HPP

#include "drako/core/meta/dk_reflection.hpp"

namespace drako::meta
{
    // MACRO
    // Marks a field as serializable.
    #define DRAKO_SERIALIZE(Field)

    // MACRO: Generates code for binary serialization.
    // Only fields marked as serializable are included.
    #define DRAKO_BUILTIN_BINARY_SERIALIZABLE(Type, ...)


    // Stream of raw bytes.
    //
    class byte_stream
    { };


    // MACRO: Generates code for binary deserialization.
    // Only fields marked as deserializable are included.
    // #define DRAKO_BUILTIN_BINARY_DESERIALIZABLE(...)

    inline byte_stream& operator<<(byte_stream& os, const uint32_t& obj) noexcept
    {
        //os.write_bytes(obj);
        return os;
    }

    inline byte_stream& operator>>(byte_stream& is, uint32_t& obj) noexcept
    {
        //obj = is.read_bytes<uint32_t>();
        return is;
    }

    /*
    template <typename Res, typename ...Ts>
    inline byte_stream& serialize(byte_stream& stream, const Res& arg, const Ts&... args)
    {
        if constexpr (sizeof...(Ts) == 0) // Last field to serialize
        {
            stream << arg;
            return stream;
        }
        else // Iterate on remaining fields
        {
            stream << arg;
            return serialize(stream, args...);
        }
    }


    template <typename Res, typename ...Ts>
    inline byte_stream& deserialize(byte_stream& stream, Res& arg, Ts&... args)
    {
        if constexpr (sizeof...(Ts) == 0)
        {
            stream >> arg;
            return stream;
        }
        else
        {
            stream >> arg;
            return deserialize(stream, args...);
        }
    }
    */
}

#endif // !DRAKO_SERIALIZATION_HPP