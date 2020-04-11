#pragma once
#ifndef DRAKO_MEMORY_STREAM_HPP
#define DRAKO_MEMORY_STREAM_HPP

#include <memory>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"

namespace drako
{
    // Stream of data backed by primary memory.
    template <typename Alloc = std::allocator<std::byte>>
    class memory_stream
    {
    public:
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

        ~memory_stream() noexcept
        {
            if (_buff != nullptr)
                std::allocator_traits<Alloc>::deallocate(_alloc, _buff, _size);
        }

        memory_stream(const memory_stream&);
        memory_stream& operator=(const memory_stream&);

        memory_stream(memory_stream&&) noexcept;
        memory_stream& operator=(memory_stream&&) noexcept;

        // Returns a pointer to the underliyng buffer.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr const std::byte* data() const noexcept
        {
            return _buff;
        }

        // Returns a pointer to the underliyng buffer.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr std::byte* data() noexcept
        {
            return _buff;
        }

        // Byte count that are currently stored.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr size_t size() const noexcept
        {
            return _size;
        }

        void read(std::byte* dst, size_t bytes);

    private:
        allocator_type& _alloc;
        size_t          _size;
        std::byte*      _buff;
        std::byte*      _seek;
    };

    template <typename Alloc>
    memory_stream<Alloc>::memory_stream(const memory_stream& rhs)
        : _alloc(std::allocator_traits<Alloc>::select_on_container_copy_construction(rhs._alloc))
        , _size(rhs._size)
        , _buff(std::allocator_traits<Alloc>::allocate(_alloc, _size))
        , _seek(buff)
    {
    }

    template <typename Alloc>
    auto memory_stream<Alloc>::operator=(const memory_stream& rhs) -> memory_stream&;

    template <typename Alloc>
    memory_stream<Alloc>::memory_stream(memory_stream&& rhs) noexcept
        : _alloc(std::move(rhs._alloc))
        , _size(std::move(rhs._size))
        , _buff(std::move(rhs._buff))
        , _seek(_buff)
    {
    }

    template <typename Alloc>
    auto memory_stream<Alloc>::operator=(memory_stream&& rhs) noexcept -> memory_stream&;

    template <typename Alloc>
    void memory_stream<Alloc>::read(std::byte* dst, size_t bytes)
    {
        DRAKO_ASSERT(dst != nullptr);
        DRAKO_ASSERT(_seek != nullptr);

        std::memcpy(dst, _seek, bytes * sizeof(std::byte));
        _seek += bytes * sizeof(std::byte);
    }

    /*
    template <typename Allocator>
    class memory_view
    {
    public:
        const std::byte*  data;
        const std::size_t size;

    private:
        Allocator& _alloc;
    };
    */

} // namespace drako

#endif // !DRAKO_MEMORY_STREAM_HPP