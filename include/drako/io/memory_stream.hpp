#pragma once
#ifndef DRAKO_MEMORY_STREAM_HPP
#define DRAKO_MEMORY_STREAM_HPP

#include <cassert>
#include <memory>
#include <span>
#include <stdexcept>

namespace drako::io
{
    class basic_input_memory_stream
    {
    public:
        virtual void read(std::byte* dst, std::size_t bytes);
        virtual void read(std::span<std::byte> dst);
    };

    class basic_output_memory_stream
    {
    public:
        virtual void write(const std::byte* src, std::size_t bytes);
        virtual void write(const std::span<std::byte> src);
    };



    /// @brief Data stream interface backed by volatile memory.
    //template <typename Alloc = std::allocator<std::byte>>
    class memory_stream
    {
    public:
        /*
        using allocator_type = Alloc;

        explicit memory_stream(Alloc& al = Alloc()) noexcept(noexcept(Alloc()))
            : _alloc(al)
            , _buff(nullptr)
            , _seek(nullptr)
            , _size(0)
        {
        }

        explicit memory_stream(size_t size, Alloc& al = Alloc()) noexcept(std::allocator_traits<Alloc>::allocate(al, size))
            : _alloc(al)
            , _size(size)
            , _buff(std::allocator_traits<Alloc>::allocate(_alloc, size))
            , _seek(_buff)
        {
        }
        */

        ~memory_stream() noexcept;

        memory_stream(const memory_stream&);
        memory_stream& operator=(const memory_stream&);

        memory_stream(memory_stream&&) noexcept;
        memory_stream& operator=(memory_stream&&) noexcept;

        // Byte count that are currently stored.
        [[nodiscard]] constexpr size_t size() const noexcept { return _size; }

        void read(std::byte* dst, std::size_t bytes)
        {
            assert(_seek != nullptr);
            assert(dst != nullptr);

            std::memcpy(dst, _seek, bytes);
            _seek += bytes;
        }

    private:
        std::size_t _size;
        std::byte*  _buff;
        std::byte*  _seek;
    };


    /// @brief Reads bytes from a memory backed stream.
    class memory_stream_reader
    {
    public:
        explicit constexpr memory_stream_reader(std::span<const std::byte> bytes) noexcept
            : _span{ bytes.data() }
            , _seek{ bytes.data() }
            , _last{ bytes.data() + bytes.size_bytes() } {}

        void read(std::byte* dst, std::size_t bytes)
        {
            if ((_seek + bytes) > _last)
                throw std::runtime_error{ "Read past the end." };

            std::memcpy(dst, _seek, bytes);
            _seek += bytes;
        }


    private:
        const std::byte* const _span; // valid memory range is [_span, _last)
        const std::byte* const _last;
        const std::byte*       _seek; // current reader position
    };


    /// @brief Writes bytes to a memory backed stream.
    class memory_stream_writer
    {
        // TODO: impl
    };

} // namespace drako::io

#endif // !DRAKO_MEMORY_STREAM_HPP