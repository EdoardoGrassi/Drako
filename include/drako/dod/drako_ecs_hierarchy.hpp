#ifndef DRAKO_ECS_HIERARCHY_HPP_
#define DRAKO_ECS_HIERARCHY_HPP_

#include <vector>
#include <unordered_map>

#include "drako_ecs_command_queue.hpp"

namespace drako::ecs
{
    /// <summary>Unique id that reference a node.</summary>
    using NodeID = std::uint32_t;

    template <typename TComponent>
    struct HierarchyNode
    {
        TComponent parentIndex;
    };

    /// <summary>Defines a managed hierarchy of nodes with father-child relationships.</summary>
    template <typename TComponent>
    class Hierarchy
    {
    public:

        /// <summary>Constructor.</summary>
        explicit Hierarchy(std::size_t reservedSize)
        {
            m_indexTable.reserve(reservedSize);
            m_hierarchy.reserve(reservedSize);
        }

        /// <summary>Destructor.</summary>
        ~Hierarchy() = default;

        HierarchyNode<TComponent> operator[](std::size_t idx) const { return m_hierarchy[idx]; }

        /// <summary>Inserts the node at root level.</summary>
        /// <remarks>The operation is delayed until the pending queue is manually executed.</remarks>
        void add_node(NodeID node)
        {
            // Insert at nodes end
            m_hierarchy.push_back(node);
            m_indexTable.insert(node, m_hierarchy.size - 1);
        }

        /// <summary>Inserts the node as a child of specified parent.</summary>
        /// <remarks>The operation is delayed until the pending queue is manually executed.</remarks>
        void add_node(NodeID node, NodeID parent)
        {
            // Insert at childs end
            std::size_t parentIndex = m_indexTable.at(parent);
            m_hierarchy.
        }

        /// <summary>Removes the node.</summary>
        /// <remarks>The operation is delayed until the pending queue is manually executed.</remarks>
        void remove_node(NodeID node)
        {

        }

        /// <summary>Updates the hierarchy state executing all pending operations</summary>
        void execute_queue()
        {

        }

    private:

        enum class HierarchyCommand
        {
            ADD_NODE,
            REMOVE_NODE
        };

        std::unordered_map<NodeID, std::size_t> m_indexTable;
        std::vector<HierarchyNode<TComponent>> m_hierarchy;
    };
}

#endif // !DRAKO_ECS_HIERARCHY_HPP_

