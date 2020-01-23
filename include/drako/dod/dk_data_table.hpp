#pragma once
#ifndef DRAKO_DATA_TABLE_HPP
#define DRAKO_DATA_TABLE_HPP

#include "core/dk_vector.hpp"
#include "core/dk_compiler_spec.hpp"

namespace drako::dod
{
    // CLASS TEMPLATE
    // Data table that stores records with an associated key.
    template <typename TKey, typename TVal>
    class DataTable final
    {
        static_assert(std::is_integral_v<TKey>, "Key type must be an integer");

    public:

        explicit DataTable(const size_t size);
        ~DataTable() noexcept = default;

        DataTable(DataTable const&) = default;
        DataTable& operator=(DataTable const&) = default;

        DataTable(DataTable&&) noexcept = default;
        DataTable& operator=(DataTable&&) noexcept = default;

        // Records count.
        DRAKO_FORCE_INLINE constexpr size_t size() const noexcept { return _keys.size(); }

        inline TKey key(size_t const idx) const noexcept;
        inline TKey& key(size_t const idx) noexcept;

        inline TVal value(size_t const idx) const noexcept;
        inline TVal& value(size_t const idx) noexcept;

        void insert(const TKey key, const TVal value);
        void insert(const TKey key, const TVal& value);
        void insert(const TKey key, TVal&& value);

        void remove(const TKey key);

        const drako::FastVector<TKey>& keys() const noexcept { return _keys; }
        const drako::FastVector<TKey>& values() const noexcept { return _vals; }

    private:

        drako::FastVector<TKey> _keys;
        drako::FastVector<TVal> _vals;
    };

    template <typename TKey, typename TVal>
    DataTable<TKey, TVal>::DataTable(size_t const size) : _keys(size), _vals(size)
    {}

    template <typename TKey, typename TVal>
    TKey DataTable<TKey, TVal>::key(const size_t idx) const
    {
        return _keys[idx];
    }

    template <typename TKey, typename TVal>
    TKey& DataTable<TKey, TVal>::key(const size_t idx)
    {
        return _keys[idx];
    }

    template <typename TKey, typename TVal>
    TVal DataTable<TKey, TVal>::value(const size_t idx) const
    {
        return _vals[idx];
    }

    template <typename TKey, typename TVal>
    TVal& DataTable<TKey, TVal>::value(const size_t idx)
    {
        return _vals[idx];
    }

    template <typename TKey, typename TVal>
    void DataTable<TKey, TVal>::insert(TKey const key, TVal const& value) noexcept
    {
        this->_keys.insert_end(key);
        this->_vals.insert_end(value);
    }

    template <typename TKey, typename TVal>
    void DataTable<TKey, TVal>::remove(TKey const k) noexcept
    {
        // Find index of key

        // Move last record to index record
    }
}

#endif // !DRAKO_DATA_TABLE_HPP
