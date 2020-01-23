#ifndef _DRAKO_RESOURCES_MANAGER_HPP_
#define _DRAKO_RESOURCES_MANAGER_HPP_

#include <unordered_map>
#include <memory>
#include <future>

namespace drako::core
{
    using DkResourceID = std::uint32_t;

    struct DkResourceAllocationBlock
    {
        void* resource_ptr;
        std::size_t nReferencesCount;
    };

    class DkResourceManager
    {
    public:
        explicit DkResourceManager();
        ~DkResourceManager() = default;

        DkResourceManager(const DkResourceManager&) = delete;
        DkResourceManager& operator=(const DkResourceManager&) = delete;

        template <typename T>
        std::unique_ptr<T> LoadUniqueResource(const DkResourceID resource);

        template <typename T>
        std::future<std::unique_ptr<T>> LoadUniqueResourceAsync(const DkResourceID resource);

        template <typename T>
        std::shared_ptr<T> LoadSharedResource(const DkResourceID resource);

        template <typename T>
        std::future<std::shared_ptr<T>> LoadSharedResourceAsync(const DkResourceID resource);

    private:

        std::unordered_map<DkResourceID, std::shared_ptr<>> m_cache;
    };

    /// <summary>Utility function that extracts an id from a resource name.</summary>
    inline constexpr DkResourceID get_id_from_resource_name(const std::string_view& resource) noexcept
    {
        return static_cast<DkResourceID>(std::hash_value(resource));
    }

    #pragma region drako_resources_management.tpp

    template <typename T>
    std::unique_ptr<T> DkResourceManager::LoadUniqueResource(const DkResourceID resource)
    {

    }

    template <typename T>
    std::future<std::unique_ptr<T>> DkResourceManager::LoadUniqueResourceAsync(const DkResourceID resource)
    {

    }

    #pragma endregion
}

#endif // !_DRAKO_RESOURCES_MANAGER_HPP_

