#pragma once
#ifndef DRAKO_ASSET_TYPES_HPP
#define DRAKO_ASSET_TYPES_HPP

#include "drako/core/drako_api_defs.hpp"
#include "drako/core/typed_handle.hpp"
#include "drako/devel/uuid.hpp"

#include <cstddef>
#include <filesystem>
#include <span>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace drako
{
    //DRAKO_DEFINE_TYPED_ID(AssetID, std::uint32_t);
    //DRAKO_DEFINE_TYPED_ID(AssetBundleID, std::uint32_t);
    using AssetID       = Uuid; // allows direct usage with the editor assets
    using AssetBundleID = Uuid; // allows direct usage with the editor bundles


    /// @brief Single item of an asset manifest.
    struct AssetManifestRecord
    {
        AssetID       asset_guid;
        uint32_t      packed_size_bytes;
        uint32_t      unpacked_size_bytes;
        AssetBundleID bundle_guid;
    };


    enum class asset_type : std::uint8_t
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
    class AssetLoadInfo
    {
    public:
        /// @brief Descriptor of an asset packaged uncompressed.
        ///
        /// @param[in] offset Offset inside the full package as bytes.
        /// @param[in] bytes  Size of the asset as bytes.
        ///
        /// @return
        explicit AssetLoadInfo(std::uint32_t offset, std::uint32_t bytes) noexcept
            : _package_offset{ offset }
            , _packed_size_bytes{ bytes }
            , _unpacked_size_bytes{ bytes }
            , _storage_flags{ asset_storage_flags::uncompressed }
            , _format_flags{} {}

        /// @brief Descriptor of a packaged asset.
        ///
        /// @param[in] offset   Offset inside the full package as bytes.
        /// @param[in] packed   Size of the asset before decompression as bytes.
        /// @param[in] unpacked Size of the asset after decompression as bytes.
        /// @param[in] s        Storage options.
        /// @param[in] f        Format options.
        ///
        /// @return
        explicit AssetLoadInfo(std::uint32_t offset, std::uint32_t packed, std::uint32_t unpacked,
            asset_storage_flags s, asset_format_flags f) noexcept
            : _package_offset{ offset }
            , _packed_size_bytes{ packed }
            , _unpacked_size_bytes{ unpacked }
            , _storage_flags{ s }
            , _format_flags{ f } {}

        // Identifier of the package.
        [[nodiscard]] std::size_t package_guid() const noexcept;

        // Byte offset inside the package.
        [[nodiscard]] std::size_t package_offset_bytes() const noexcept;

        // Byte size of the uncompressed asset.
        [[nodiscard]] std::size_t unpacked_size_bytes() const noexcept;

        // Byte size of the compressed asset.
        [[nodiscard]] std::size_t packed_size_bytes() const noexcept;

        // Storage settings.
        [[nodiscard]] asset_storage_flags storage_flags() const noexcept;

        // Format of the data payload.
        [[nodiscard]] asset_format_flags format_flags() const noexcept;

    private:
        std::uint32_t       _package_offset;
        std::uint32_t       _packed_size_bytes;
        std::uint32_t       _unpacked_size_bytes;
        asset_storage_flags _storage_flags;
        asset_format_flags  _format_flags;
    };
    static_assert(sizeof(AssetLoadInfo) == 16,
        "Bad class layout: required internal padding bits.");
    static_assert(alignof(AssetLoadInfo) <= sizeof(AssetLoadInfo),
        "Bad class layout: required external padding bits.");


    /// @brief Metadata descriptor of an asset bundle.
    struct AssetBundleMeta
    {
        // Name of the bundle.
        char name[16];

        // Path to storage file.
        char path[16];

        // Size in bytes of bundle data in storage file.
        std::uint64_t size;

        // Offset in bytes relative to begin of storage file.
        std::uint64_t offset;
    };



    struct AssetBundle
    {

    public:
        void load_from_file();

        void save_to_file();
    };


    /// @brief Manifest file name of an asset bundle.
    [[nodiscard]] inline std::filesystem::path manifest_filename(const AssetBundleID& b)
    {
        return "bundle_" + std::to_string(b) + ".manifest.drako";
    }

    /// @brief Storage file name of an asset bundle.
    [[nodiscard]] inline std::filesystem::path storage_filename(const AssetBundleID& b)
    {
        return "bundle_" + std::to_string(b) + ".storage.drako";
    }

} // namespace drako

#endif // !DRAKO_ASSET_TYPES_HPP