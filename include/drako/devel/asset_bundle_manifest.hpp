#pragma once
#ifndef DRAKO_ASSET_BUNDLE_MANIFEST_HPP
#define DRAKO_ASSET_BUNDLE_MANIFEST_HPP

#include "drako/devel/asset_types.hpp"
#include "drako/io/input_file_handle.hpp"
#include "drako/io/output_file_handle.hpp"

#include <memory>
#include <span>

namespace drako
{
    class asset_bundle_manifest
    {
    public:
        explicit asset_bundle_manifest(const std::span<std::byte> blob);

        asset_bundle_manifest(const asset_bundle_manifest&) = delete;
        asset_bundle_manifest& operator=(const asset_bundle_manifest&) = delete;

        void load_from_file(io::input_file_handle);

        void save_to_file(io::output_file_handle);

    private:
        /// @brief Header section of an asset manifest
        struct _header_type
        {
            api_version   version = current_api_version();
            std::uint32_t crc;
            std::uint32_t items_count;
        };

        _header_type                       _header;
        std::unique_ptr<asset_id[]>        _ids;
        std::unique_ptr<asset_load_info[]> _infos;
    };

    /*
    void asset_bundle_manifest::load_from_file(io::input_file_handle is)
    {
        io::read_from_bytes(is, _header);

        // TODO: use std::make_unique_for_overwrite() when is available
        _ids = std::make_unique<asset_id[]>(_header.items_count);
        io::read_from_bytes(is, _ids.get(), _header.items_count);

        // TODO: use std::make_unique_for_overwrite() when is available
        _infos = std::make_unique<asset_load_info[]>(_header.items_count);
        io::read_from_bytes(is, _infos.get(), _header.items_count);
    }

    void asset_bundle_manifest::save_to_file(io::output_file_handle os)
    {
        io::write_to_bytes(os, _header);
        io::write_to_bytes(os, _ids.get(), _header.items_count);
        io::write_to_bytes(os, _infos.get(), _header.items_count);
    }
    */

    class manifest_view
    {
    public:
        explicit manifest_view(std::span<const std::byte> bytes)
        {
            std::memcpy(&_size, std::data(bytes), sizeof(decltype(_size)));
            if (std::size(bytes) != (sizeof(_size) + 2 * _size))
                throw std::runtime_error{ "Reported size doesn't match input buffer size." };

            _ids  = std::data(bytes) + sizeof(_size);
            _meta = std::data(bytes) + sizeof(_size) + _size;
        }

        [[nodiscard]] std::span<const std::byte> ids() const noexcept;
        [[nodiscard]] std::span<const std::byte> metadata() const noexcept;

    private:
        const std::byte* _ids;
        const std::byte* _meta;
        std::uint32_t    _size;
    };

} // namespace drako

#endif // !DRAKO_ASSET_BUNDLE_MANIFEST_HPP