#pragma once
#ifndef DRAKO_ASSET_TYPES_HPP_
#define DRAKO_ASSET_TYPES_HPP_

#include <cstddef>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "drako/core/drako_api_defs.hpp"
#include "drako/devel/uuid.hpp"

namespace drako
{
    using asset_id        = std::uint32_t;
    using asset_bundle_id = std::uint16_t;

    struct asset_manifest_header
    {
        api_version   version;
        std::uint32_t crc;
        std::uint32_t items_count;
    };

    struct asset_manifest_entry
    {
        asset_id        asset_guid;
        uint32_t        packed_size_bytes;
        uint32_t        unpacked_size_bytes;
        asset_bundle_id bundle_guid;
    };


    enum class asset_type
    {
        mesh,
        texture
    };

    enum class asset_storage_flags : std::uint8_t
    {
        uncompressed
    };

    enum class asset_format_flags : std::uint8_t
    {
    };


    // Metadata descriptor related to storage settings.
    class asset_load_info
    {
    public:
        explicit asset_load_info(asset_bundle_id bundle, std::uint32_t bytes) noexcept
            : _bundle_guid(bundle)
            , _packed_size_bytes(bytes)
            , _unpacked_size_bytes(bytes)
            , _storage_flags(asset_storage_flags::uncompressed)
            , _format_flags()
        {
        }

        // Identifier of the package.
        [[nodiscard]] size_t package_guid() const noexcept;

        // Byte offset inside the package.
        [[nodiscard]] size_t package_offset_bytes() const noexcept;

        // Byte size of the uncompressed asset.
        [[nodiscard]] size_t unpacked_size_bytes() const noexcept;

        // Byte size of the compressed asset.
        [[nodiscard]] size_t packed_size_bytes() const noexcept;

        // Storage settings.
        [[nodiscard]] asset_storage_flags storage_flags() const noexcept;

        // Format of the data payload.
        [[nodiscard]] asset_format_flags format_flags() const noexcept;

    private:
        std::uint32_t       _package_offset;
        std::uint32_t       _packed_size_bytes;
        std::uint32_t       _unpacked_size_bytes;
        asset_bundle_id     _bundle_guid;
        asset_storage_flags _storage_flags;
        asset_format_flags  _format_flags;
    };
    static_assert(sizeof(asset_file_info) == 16 * sizeof(std::byte),
        "Bad class layout: required internal padding bits");
    static_assert(alignof(asset_file_info) <= sizeof(asset_file_info),
        "Bad class layout: required external padding bits");



    class serialized_component_array
    {
    public:
    private:
        std::uint32_t _component_type_id;
        std::uint32_t _size;
        std::uint8_t  _stride;
    };

    class serialized_instance_array_view
    {
        static_assert(std::is_trivially_constructible_v<serialized_component_array>,
            "Required for direct construction with memory copy");

    public:
        explicit serialized_instance_array_view(std::byte* data, size_t size) noexcept
            : _data(data)
            , _size(size)
        {
        }

        [[nodiscard]] const serialized_component_array* cbegin() const noexcept
        {
            return reinterpret_cast<serialized_component_array*>(_data);
        }

        [[nodiscard]] const serialized_component_array* cend() const noexcept
        {
            return reinterpret_cast<serialized_component_array*>(_data) + _size;
        }

        [[nodiscard]] serialized_component_array* begin() noexcept
        {
            return reinterpret_cast<serialized_component_array*>(_data);
        }

        [[nodiscard]] serialized_component_array* end() noexcept
        {
            return reinterpret_cast<serialized_component_array*>(_data) + _size;
        }

    private:
        std::uint32_t _size;
        std::byte*    _data;
    };


    struct asset_group
    {
        std::vector<asset_bundle_id> required_bundles;

        std::vector<asset_id>        asset_instance_id;
        std::vector<asset_bundle_id> asset_bundle_id;
    };

} // namespace drako

#endif // !DRAKO_ASSET_TYPES_HPP_