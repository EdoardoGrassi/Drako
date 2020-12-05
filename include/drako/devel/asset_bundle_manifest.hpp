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
    class AssetBundleManifest
    {
    public:
        explicit AssetBundleManifest(const std::span<std::byte> blob);

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

        _header                            _header;
        std::unique_ptr<AssetID[]>         _ids;
        std::unique_ptr<asset_load_info[]> _infos;
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
            assert(std::size(bytes) > sizeof(_header));
            std::memcpy(&_size, std::data(bytes), sizeof(decltype(_size)));
            if (std::size(bytes) != (sizeof(_size) + 2 * _size))
                throw std::runtime_error{ "Reported size doesn't match input buffer size." };

            _ids  = std::data(bytes) + sizeof(_size);
            _meta = std::data(bytes) + sizeof(_size) + _size;
        }

        [[nodiscard]] std::span<const std::byte> ids() const noexcept
        {
            return { _ids, _size };
        }
        [[nodiscard]] std::span<const std::byte> metadata() const noexcept
        {
            return { _meta, _size };
        }

    private:
        struct _header
        {
            std::uint32_t size;

            _header(std::span<const std::byte> bytes)
                : size{ from_le_bytes<decltype(size)>(std::data(bytes)) } {}
        };

        const std::byte* _ids;
        const std::byte* _meta;
        std::size_t      _size;
    };

} // namespace drako

#endif // !DRAKO_ASSET_BUNDLE_MANIFEST_HPP