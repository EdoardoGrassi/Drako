#pragma once
#ifndef DRAKO_ASSET_BUNDLE_TYPES_HPP
#define DRAKO_ASSET_BUNDLE_TYPES_HPP

#include "drako/core/byte_utils.hpp"
#include "drako/devel/asset_types.hpp"
#include "drako/devel/version.hpp"

#include <yaml-cpp/yaml.h>

#include <cassert>
#include <filesystem>
#include <iosfwd>
#include <memory>
#include <span>
#include <string>

namespace drako
{
    /// @brief Metadata of an asset bundle
    struct AssetBundleInfo
    {
        std::string   name;
        AssetBundleID guid;
    };

    const YAML::Node& operator>>(const YAML::Node&, AssetBundleInfo&);
    YAML::Node&       operator<<(YAML::Node&, const AssetBundleInfo&);


    struct AssetBundleImportError
    {
        std::filesystem::path file;
        std::string           message;
    };


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


    /// @brief Manifest for the content of a bundle.
    struct AssetBundleManifest
    {
        static_assert(std::is_trivially_copyable_v<AssetID>,
            "Required for direct serialization in memory.");
        static_assert(std::is_trivially_copyable_v<AssetLoadInfo>,
            "Required for direct serialization in memory.");

    public:
        /// @brief Header section of a serialized asset manifest.
        struct Header
        {
            Version       version = current_api_version();
            std::string   name;
            AssetBundleID uuid;
            //std::uint32_t crc;
            //std::uint32_t items_count;

            [[nodiscard]] bool operator==(const Header&) const& noexcept = default;
        };
        //static_assert(std::is_trivially_copyable_v<Header>,
        //    "Required for direct serialization in memory.");

        explicit AssetBundleManifest() = default;
        explicit AssetBundleManifest(std::span<const std::byte> data);

        //AssetBundleManifest(const AssetBundleManifest&) = delete;
        //AssetBundleManifest& operator=(const AssetBundleManifest&) = delete;

        //friend std::istream& operator>>(std::istream&, AssetBundleManifest&);
        //friend std::ostream& operator<<(std::ostream&, const AssetBundleManifest&);

        Header               header;
        std::vector<AssetID> guids;
        //std::vector<AssetLoadInfo> infos;

        [[nodiscard]] bool operator==(const AssetBundleManifest&) const& noexcept;
    };

    std::istream& operator>>(std::istream&, AssetBundleManifest&);
    std::ostream& operator<<(std::ostream&, const AssetBundleManifest&);

    const YAML::Node& operator>>(const YAML::Node&, AssetBundleManifest&);
    YAML::Node&       operator<<(YAML::Node&, const AssetBundleManifest&);


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

    /// @brief Convert a bundle ID to its associated manifest file name.
    [[nodiscard]] inline std::filesystem::path bundle_meta_filename(const AssetBundleID& b)
    {
        return "bundle_" + std::to_string(b) + ".manifest.drako";
    }

    /// @brief Convert a bundle ID to its associated storage file name.
    [[nodiscard]] inline std::filesystem::path bundle_data_filename(const AssetBundleID& b)
    {
        return "bundle_" + std::to_string(b) + ".storage.drako";
    }

} // namespace drako

#endif // !DRAKO_ASSET_BUNDLE_TYPES_HPP