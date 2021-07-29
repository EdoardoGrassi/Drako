#pragma once
#ifndef DRAKO_BYTE_STREAM_HPP
#define DRAKO_BYTE_STREAM_HPP

#include <cstddef>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace drako
{
    /// @brief Input stream interface over a raw sequence of bytes.
    class InputByteStream
    {
    public:
        //explicit InputByteStream() = default;

        explicit InputByteStream(const std::size_t capacity)
            : _buff{ capacity }
            , _head{ std::data(_buff) }
        {
        }

        template <typename T>
        requires std::is_trivially_copyable_v<T>
            InputByteStream& operator>>(T& t)
        {
            const auto bytes = sizeof(T);
            if (_free_bytes() < bytes)
                throw std::runtime_error{ "Buffer out of data" };

            std::memcpy(static_cast<std::byte*>(std::addressof(t)), _head, bytes);
            _head += bytes;
            return this;
        }

        template <typename T>
        requires std::is_trivially_copyable_v<T>
            InputByteStream& operator>>(const std::span<T> s)
        {
            const auto bytes = sizeof(T) * std::size(s);
            if (_free_bytes < bytes)
                throw std::runtime_error{ "Buffer out of data" };

            std::memcpy(static_cast<std::byte*>(std::data(s)), _head, bytes);
            _head += bytes;
            return this;
        }

        //template <typename T>
        //void read(std::span<T>);

    private:
        std::vector<std::byte> _buff;
        std::byte*             _head;

        [[nodiscard]] std::size_t _free_bytes()
        {
            return std::distance(_head, std::data(_buff) + std::size(_buff));
        }
    };


    /// @brief Output stream interface over a raw sequence of bytes.
    class OutputByteStream
    {
    public:
        explicit OutputByteStream(const std::size_t capacity)
            : _buff{ capacity }
            , _head{ std::data(_buff) }
        {
        }

        template <typename T>
        requires std::is_trivially_copyable_v<T>
            OutputByteStream& operator<<(const T& t)
        {
            const auto bytes = sizeof(T);
            _buff.resize(std::size(_buff) + bytes);
            std::memcpy(_head, std::addressof(t), bytes);
            _head += bytes;
            return this;
        }

    private:
        std::vector<std::byte> _buff;
        std::byte*             _head;
    };

} // namespace drako

#endif // !DRAKO_BYTE_STREAM_HPP