#ifndef DRAKO_ECS_ENTITY_WORLD_HPP
#define DRAKO_ECS_ENTITY_WORLD_HPP

#include <tuple>

#include "drako_ecs_entity.hpp"

namespace drako::dod
{

    /// <summary>Defines an encapsulated scope of existence for a group of entities of different archetypes.</summary>
    template <typename ...Args>
    class EntityWorld final
    {
    public:

        explicit EntityWorld() noexcept {}

        explicit EntityWorld(const EntityWorld& rhs) = delete;
        EntityWorld& operator=(const EntityWorld& rhs) = delete;

        template <typename ...TComponents>
        constexpr EntityID create_entity(const TComponents ...components) noexcept;

        constexpr void destroy_entity(const EntityID id) noexcept;

        constexpr bool exists(const EntityID id) const noexcept;

    private:

        EntityID m_lastEntityId = 0;
    };


    template<typename ...Args>
    template<typename ...TComponents>
    constexpr EntityID EntityWorld<Args...>::create_entity(const TComponents ...components) noexcept
    {
        EntityID id = ++m_lastEntityId;

        // Enqueue entity for creation

        return id;
    }

    template<typename ...Args>
    constexpr void EntityWorld<Args...>::destroy_entity(const EntityID id) noexcept
    {
        // TODO: assert id is valid

        // Enqueue for remove
    }

    template<typename ...Args>
    constexpr bool EntityWorld<Args...>::exists(const EntityID id) const noexcept
    {
        // TODO: assert id is valid

        return false;
    }
}

#endif // !DRAKO_ECS_ENTITY_WORLD_HPP
