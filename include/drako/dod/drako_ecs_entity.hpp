#ifndef DRAKO_ECS_ENTITY_HPP_
#define DRAKO_ECS_ENTITY_HPP_

#include <cstdint>
#include <typeinfo>

#include "drako_ecs_table.hpp"

namespace drako::dod
{
    /// <summary>Unique id that identifies a single entity.</summary>
    using EntityID = std::uint32_t;


    /// <summary>Represent a unique configuration of components.</summary>
    template <typename TKey, typename ...TComponents>
    class EntityArchetype
    {
    public:

        explicit constexpr EntityArchetype() noexcept;
        ~EntityArchetype() noexcept = default;

        constexpr EntityArchetype(const EntityArchetype& rhs) noexcept = default;
        constexpr EntityArchetype& operator=(const EntityArchetype& rhs) noexcept = default;

    private:

        DataTable<TKey, TComponents...> m_entities;
    };


    template <typename TKey, typename ...TComponents>
    inline constexpr EntityArchetype<TKey, TComponents...>::EntityArchetype()
    {
        //m_signatureHash = (typeid(Args).hash_code() ^ ...); NICE HACKS
    }
}

#endif // !DRAKO_ECS_ENTITY_HPP_

