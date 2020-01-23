#pragma once
#ifndef DRAKO_ASSET_ARCHIVE_HPP
#define DRAKO_ASSET_ARCHIVE_HPP

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>

#include "drako/core/preprocessor/compiler_macros.hpp"

namespace drako
{
    // Uniquely identifies an asset.
    class asset_guid
    {
    public:
        explicit constexpr asset_guid(uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4) noexcept
            : _hash{ h1, h2, h3, h4 }
        {
        }

        explicit constexpr asset_guid(uint32_t hash[4]) noexcept
            : _hash{ hash[0], hash[1], hash[2], hash[3] }
        {
        }

        DRAKO_NODISCARD constexpr bool operator==(const asset_guid& rhs) const noexcept
        {
            return (_hash[0] == rhs._hash[0]) &
                   (_hash[1] == rhs._hash[1]) &
                   (_hash[2] == rhs._hash[2]) &
                   (_hash[3] == rhs._hash[3]);
        }

        DRAKO_NODISCARD constexpr bool operator!=(const asset_guid& rhs) const noexcept
        {
            return (_hash[0] != rhs._hash[0]) |
                   (_hash[1] != rhs._hash[1]) |
                   (_hash[2] != rhs._hash[2]) |
                   (_hash[3] != rhs._hash[3]);
        }

        std::string friend to_string(const asset_guid&);

    private:
        const std::uint32_t _hash[4];
    };

    std::string to_string(const asset_guid& obj)
    {
        return std::to_string(obj._hash[0]) +
               std::to_string(obj._hash[1]) +
               std::to_string(obj._hash[2]) +
               std::to_string(obj._hash[3]);
    }


    enum class asset_storage_flags : std::uint16_t
    {

    };

    enum class asset_format_flags : std::uint16_t
    {
    };


    // Metadata descriptor related to storage settings.
    class asset_desc
    {
    public:
        explicit asset_desc() noexcept;

        // Identifier of the package.
        std::uint32_t package_guid;

        // Byte offset inside the package.
        // std::uint32_t package_offset;

        // Byte size of the compressed asset.
        std::uint32_t packed_size_bytes;

        // Byte size of the uncompressed asset.
        std::uint32_t unpacked_size_bytes;

        // Storage settings.
        asset_storage_flags storage_flags;

        // Format of the data payload.
        asset_format_flags format_flags;

        DRAKO_NODISCARD size_t package_guid() const noexcept;

        DRAKO_NODISCARD size_t package_offset_bytes() const noexcept;

        DRAKO_NODISCARD size_t unpacked_size_bytes() const noexcept;

        DRAKO_NODISCARD size_t packed_size_bytes() const noexcept;

        DRAKO_NODISCARD asset_storage_flags storage_flags() const noexcept;

        DRAKO_NODISCARD asset_format_flags format_flags() const noexcept;
    };
    static_assert(sizeof(asset_desc) == 16 * sizeof(std::byte),
        "Bad class layout : required internal padding bits");
    static_assert(alignof(asset_desc) <= sizeof(asset_desc),
        "Bad class layout : required external padding bits");


    // Stores metadata about assets.
    template <typename Allocator = std::allocator<asset_desc>>
    class asset_register
    {
    public:
        using guid_type = asset_guid;
        using desc_type = asset_desc;

        explicit asset_register(const std::vector<desc_type>& descriptors)
            : _descriptors(descriptors)
        {
        }

        explicit runtime_asset_register();

        bool insert(const guid_type& id, const desc_type& desc) noexcept;

        bool remove(const guid_type& id) noexcept;

        bool update(const guid_type& id, const desc_type& desc) noexcept;

        DRAKO_NODISCARD std::tuple<const desc_type&, bool>
        find(const guid_type& guid) const noexcept;

    private:
        std::vector<desc_type> _descriptors;
    };

    using runtime_asset_database = std::unordered_map<uint32_t, asset_desc>;

} // namespace drako

#endif // !DRAKO_ASSET_ARCHIVE_HPP