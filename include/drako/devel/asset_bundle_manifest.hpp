#pragma once
#ifndef DRAKO_ASSET_BUNDLE_MANIFEST_HPP
#define DRAKO_ASSET_BUNDLE_MANIFEST_HPP

#include "drako/core/byte_utils.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/io/input_file_handle.hpp"
#include "drako/io/output_file_handle.hpp"

#include <cassert>
#include <memory>
#include <span>

namespace drako
{
    class AssetBundleListView
    {
    public:
        explicit AssetBundleListView(std::span<const std::byte> data)
            : _data(std::data(data))
        {
            std::memcpy(&_h, std::data(data), sizeof(_h));
        }

        [[nodiscard]] std::size_t size() const noexcept { return _h.row_count; }

        [[nodiscard]] std::span<const std::byte> ids() const noexcept
        {
            return { _data, _h.row_count };
        }

    private:
        struct _header
        {
            std::uint32_t total_data_size;
            std::uint32_t row_count;
            std::uint32_t id_column_offset;
            //std::uint32_t name_column_offset;
        };
        _header          _h;
        const std::byte* _data;
    };

    class AssetBundleManifest
    {
    public:
        explicit AssetBundleManifest(std::span<const std::byte> data);

        //AssetBundleManifest(const AssetBundleManifest&) = delete;
        //AssetBundleManifest& operator=(const AssetBundleManifest&) = delete;

        void load_from_file(io::UniqueInputFile);

        void save_to_file(io::UniqueOutputFile);

    private:
        /// @brief Header section of an asset manifest
        struct _header
        {
            Version       version = current_api_version();
            std::uint32_t crc;
            std::uint32_t items_count;
        };

        _header                          _header;
        std::unique_ptr<AssetID[]>       _ids;
        std::unique_ptr<AssetLoadInfo[]> _infos;
    };

    /*
    void asset_bundle_manifest::load_from_file(io::UniqueInputFile is)
    {
        io::read_from_bytes(is, _header);

        // TODO: use std::make_unique_for_overwrite() when is available
        _ids = std::make_unique<asset_id[]>(_header.items_count);
        io::read_from_bytes(is, _ids.get(), _header.items_count);

        // TODO: use std::make_unique_for_overwrite() when is available
        _infos = std::make_unique<asset_load_info[]>(_header.items_count);
        io::read_from_bytes(is, _infos.get(), _header.items_count);
    }

    void asset_bundle_manifest::save_to_file(io::UniqueOutputFile os)
    {
        io::write_to_bytes(os, _header);
        io::write_to_bytes(os, _ids.get(), _header.items_count);
        io::write_to_bytes(os, _infos.get(), _header.items_count);
    }
    */

    class AssetBundleManifestView
    {
    public:
        explicit AssetBundleManifestView(std::span<const std::byte> bytes)
        {
            if (std::size(bytes) > sizeof(_header))
                throw std::runtime_error{ "Input bytes can't represent a valid manifest." };

            std::memcpy(&_header, std::data(bytes), sizeof(_header));
            if (std::size(bytes) != (sizeof(_header.size) + _header.size * (sizeof(AssetID) + sizeof(AssetLoadInfo))))
                throw std::runtime_error{ "Reported size doesn't match input buffer size." };

            _ids  = std::data(bytes) + sizeof(_header_type);
            _meta = std::data(bytes) + sizeof(_header_type) + _header.size * sizeof(AssetID);
        }

        [[nodiscard]] std::size_t size() const noexcept { return _header.size; }

        [[nodiscard]] std::span<const std::byte> ids() const noexcept
        {
            return { _ids, _header.size };
        }

        [[nodiscard]] std::span<const std::byte> metadata() const noexcept
        {
            return { _meta, _header.size };
        }

    private:
        struct _header_type
        {
            std::uint32_t size;
            std::uint32_t ids_column_offset;
            std::uint32_t meta_column_offset;

            //_header_type(std::span<const std::byte> bytes)
            //    : size{ from_le_bytes<decltype(size)>(std::data(bytes)) } {}
        };
        static_assert(std::is_trivially_copyable_v<_header_type>);

        _header_type     _header;
        const std::byte* _ids;
        const std::byte* _meta;
    };

} // namespace drako

#endif // !DRAKO_ASSET_BUNDLE_MANIFEST_HPP