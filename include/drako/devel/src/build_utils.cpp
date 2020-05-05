#pragma once
#include "drako/devel/build_utils.hpp"

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/devel/assertion.hpp"

#include <intrin.h>

namespace drako
{
    /*
    [[nodiscard]] std::uint32_t crc32c(const std::byte* data, const size_t size) noexcept
    {
        DRAKO_ASSERT(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;

        const auto data_as_u8 = reinterpret_cast<const uint8_t*>(data);
        for (size_t i = 0; i < (size % sizeof(uint64_t)); ++i)
            crc = _mm_crc32_u8(crc, data_as_u8[i]);

        const auto data_as_u64 = reinterpret_cast<const uint64_t*>(data + (size % sizeof(uint64_t)));
        for (size_t i = 0; i < (size / sizeof(uint64_t)); ++i)
            crc = _mm_crc32_u64(crc, data_as_u64[i]);

        return crc;
#else
#error Architecture not supported
#endif
    }
    */

    [[nodiscard]] std::uint32_t crc32c(const std::uint32_t* data, const size_t size) noexcept
    {
        DRAKO_ASSERT(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;
        for (size_t i = 0; i < size; ++i)
            crc = _mm_crc32_u32(crc, *data);
        return crc;
#else
#error Architecture not supported
#endif
    }

    [[nodiscard]] std::uint32_t crc32c(const std::uint64_t* data, const size_t size) noexcept
    {
        DRAKO_ASSERT(data != nullptr);

#if defined(DRAKO_ARCH_INTEL)
        std::uint32_t crc = 0;
        for (size_t i = 0; i < size; ++i)
            crc = _mm_crc32_u64(crc, *data);
        return crc;
#else
#error Architecture not supported
#endif
    }

    [[nodiscard]] std::variant<asset_manifest, build_error>
    build_asset_manifest(const _fs::path& build_folder);

    [[nodiscard]] std::variant<_package_result, build_error>
    build_packages_for_devel(const _fs::path& folder, const std::vector<std::string>& names, const std::vector<asset_type>& types)
    {
        using _ec  = std::error_code;
        using _apm = asset_manifest_entry;
        using _apd = asset_load_info;

        if (_ec ec; !_fs::is_directory(folder, ec))
            return build_error{};

        if (std::empty(names))
            return build_error{};
        if (std::empty(types))
            return build_error{};
        if (std::size(names) != std::size(types))
            return build_error{};

        std::vector<_apm> packages(std::size(names));
        std::vector<_apd> assets(std::size(names));
        for (auto i = 0; i < std::size(names); ++i)
        {
            _ec ec;

            const auto source = folder / names[i];
            if (!_fs::is_regular_file(source))
                return build_error{};

            if (const auto source_size = _fs::file_size(source, ec); ec)
            {
                if (source_size > std::numeric_limits<uint32_t>::max()) // file size out of range
                    return build_error{};

                const _apm package{ static_cast<uint16_t>(i) };
                packages.emplace_back(package);

                const _apd asset{ 0, static_cast<uint32_t>(source_size) };
                assets.emplace_back(asset);
            }
            else
                return build_error{};
        }
        return _package_result{ packages, assets };
    }

} // namespace drako