#pragma once
#ifndef DRAKO_RUNTIME_ASSET_LOADER_HPP
#define DRAKO_RUNTIME_ASSET_LOADER_HPP

#include <cstddef>
#include <filesystem>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <utility>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/system/memory_stream.hpp"
#include "drako/core/system/native_io_api.hpp"
#include "drako/core/system/native_io_file.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/asset_register.hpp"
#include "drako/devel/logging.hpp"

namespace drako
{
    // Uniquely identifies a resource of a specific type.
    template <typename T>
    struct guid : public asset_guid
    {
    };

    template <typename T> // , typename Alloc>
    class resource_descriptor_set
    {
    public:
        using guid_type            = guid<T>;
        using local_container_type = std::vector<T>;
        using local_index_type     = std::unordered_map<guid_type, size_t>;
        // using resource_allocator_type = Alloc;

        class iterator
        {
        };

        explicit resource_descriptor_set(size_t size) noexcept
            : _descriptors(size)
            , _guids(size)
        //, _alloc(a)
        {
        }


        DRAKO_NODISCARD DRAKO_FORCE_INLINE auto begin() noexcept
        {
            return std::begin(_descriptors);
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE const auto cbegin() const noexcept
        {
            return std::cbegin(_descriptors);
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE auto end() noexcept
        {
            return std::end(_descriptors);
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE const auto cend() const noexcept
        {
            return std::cend(_descriptors);
        }

        DRAKO_NODISCARD auto find(guid_type guid) const noexcept -> local_container_type::const_iterator
        {
            if (const auto found = _guids.find(guid); found != std::cend(_guids))
            {
                return _descriptors.cbegin() + found->second;
            }
            return std::cend(_descriptors);
        }

        DRAKO_NODISCARD auto find(guid_type guid) noexcept -> local_container_type::iterator
        {
            if (const auto found = _guids.find(guid); found != std::cend(_guids))
            {
                return std::next(std::begin(_descriptors), found->second);
            }
            return std::end(_descriptors);
        }

        template <typename... Args>
        DRAKO_NODISCARD T& make_descriptor(guid_type guid, Args&&... args) noexcept
        {
            const auto next_free_slot = std::size(_descriptors);
            if (const auto [it, result] = _guids.try_emplace(guid, next_free_slot); !result)
            {
                std::exit(EXIT_FAILURE);
            }
            return _descriptors.emplace_back(args);
        }

    private:
        local_container_type _descriptors;
        local_index_type     _guids;
        // resource_allocator_type _alloc;
    };


    // Handle to a readonly shared resource.
    template <typename T>
    class shared_resource_handle
    {
        T* _ptr;
    };


    // Handle to an owned resource.
    template <typename T>
    class unique_resource_handle
    {
        T* _ptr;
    };


    template <typename T>
    class async_request
    {
    };


    // Handles asset loading from resource packages.
    template <typename Allocator = std::allocator<std::byte>>
    class asset_loader
    {
    public:
        using guid_type            = asset_guid;
        using loader_callback_type = void(void) noexcept;

        explicit asset_loader(
            const std::vector<std::filesystem::path> packages,
            size_t                                   capacity,
            Allocator&                               al = Allocator()) noexcept
            : _allocator(al)
            , _staging(std::allocator_traits<Allocator>::allocate(al, capacity);)
        {
            DRAKO_ASSERT(capacity > 0, "No staging space available");
            DRAKO_ASSERT(!std::empty(packages), "No packages provided");

            _packages.reserve(std::size(packages));
            for (const auto& package : packages)
            {
                auto file = sys::native_file{};
                if (const auto ec = file.open(package.native(), sys::access_mode::read, sys::share_mode::exclusive);
                    ec != std::error_condition{})
                {
                    DRAKO_LOG_ERROR("[Drako] Error " + std::to_string(ec.value()) + " : " + ec.message());
                }
                _packages.emplace_back(file);
            }

            if (_staging == nullptr)
            {
                DRAKO_LOG_ERROR("[Drako] Failed memory allocation");
                std::exit(EXIT_FAILURE);
            }
        }

        asset_loader(asset_loader const&) = delete;
        asset_loader& operator=(asset_loader const&) = delete;

        asset_loader(asset_loader&&) = delete;
        asset_loader& operator=(asset_loader&&) = delete;

        template <typename ResourceAllocator>
        DRAKO_NODISCARD const memory_stream<ResourceAllocator>
        load(const asset_desc& desc, ResourceAllocator& al) noexcept;

        /*
        template <typename T, typename Alloc, typename Loader>
        DRAKO_NODISCARD unique_resource_handle<T>
        load_unique(guid_type guid, Alloc& allocator, Loader& loader) noexcept;

        template <typename T>
        DRAKO_NODISCARD shared_resource_handle<T>
        load_shared(guid_type guid) noexcept;

        template <typename T>
        DRAKO_NODISCARD async_request<T>
        load_unique_async(guid_type guid, callback_type callback) noexcept;
        */

    private:
        std::vector<sys::native_file> _packages;
        Allocator                     _allocator;
        const std::byte*              _staging;
    };

    template <typename Allocator>
    template <typename ResourceAllocator>
    DRAKO_NODISCARD const memory_stream<ResourceAllocator>
    asset_loader<Allocator>::load(const asset_desc& asset_info, ResourceAllocator& al) noexcept
    {
        auto&      src             = _sources[asset_info.package_guid()];
        const auto src_data_size   = asset_info.packed_size_bytes();
        const auto src_data_offset = asset_info.package_offset_bytes();
        const auto mem_data_size   = asset_info.unpacked_size_bytes();

        auto staging_ptr = al.allocate(src_data_size);
        if (staging_ptr == nullptr)
        {
            DRAKO_LOG_ERROR("[Drako] Failed to allocate destination memory");
            std::exit(EXIT_FAILURE);
        }
        memory_stream<staging_allocator_type> staging{ src_data_size, al };

        // read directly into destination memory
        if (const auto ec = src.read(staging.data(), src_data_size, src_data_offset);
            ec != std::error_code{})
        {
            DRAKO_LOG_ERROR("[Drako] Error " + std::to_string(ec.value()) + " : " + ec.message());
            std::exit(EXIT_FAILURE);
        }

        return staging;
    }


    template <typename T, typename GuidIterator, typename ResourceIterator, typename Loader>
    void load_resource(guid<T> id,
        GuidIterator g_first, GuidIterator g_last,
        ResourceIterator r_first, ResourceIterator r_last,
        Loader& loader) noexcept
    {
        using _g_iter_traits = std::iterator_traits<GuidIterator>;
        using _r_iter_traits = std::iterator_traits<ResourceIterator>;

        static_assert(std::is_same_v<_g_iter_traits::value_type, guid<T>>,
            "Expected an iterator to guid type guid<T>");
        static_assert(std::is_same_v<_r_iter_traits::value_type, T>,
            "Expected an iterator to resource type <T>");

        if (const auto guid = std::find(g_first, g_last, id); guid != g_last)
        { // resource already available in memory
            return;
        }
        else
        { // load from disk
            if (const auto meta = std::find(r_first, r_last, id); meta != r_last)
            {
                const auto offset     = std::distance(r_first, meta);
                const auto descriptor = std::next(std::cbegin(g_asset_infos), offset);
                const auto mesh       = loader.load(*descriptor, g_allocator);
                g_mesh_guids.emplace_back(id);
                g_mesh_assets.emplace_back(mesh);
            }
            else
            { // missing metadata entry for the required resource
                DRAKO_LOG_ERROR("[Drako] Asset descriptor of type <" DRAKO_STRINGIZE(mesh_guid) "> for ID <" + to_string(id) + "> not found");
                std::exit(EXIT_FAILURE);
            }
        }
    }

} // namespace drako

#endif // !DRAKO_RUNTIME_ASSET_LOADER_HPP