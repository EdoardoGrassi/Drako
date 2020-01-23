#ifndef DRAKO_GUI_HIERARCHY_HPP
#define DRAKO_GUI_HIERARCHY_HPP

#include <vector>

namespace drako::gui
{
    template <typename Ty>
    class Hierarchy final
    {
    public:

        constexpr explicit Hierarchy(const size_t level_count);
        ~Hierarchy() noexcept = default;

        constexpr std::vector<Ty>& operator[](const size_t idx) noexcept { return _levels[i]; }
        constexpr const std::vector<Ty>& operator[](const size_t idx) const noexcept { return _levels[i]; }

        /// <summary>Returns the index of the highest level of the hierarchy.</summary>
        constexpr size_t root_level_index() const noexcept { return 0; }

        /// <summary>Returns the index of the deepest level of the hierarchy.</summary>
        constexpr size_t leaf_level_index() const noexcept { return level_count() - 1; }

        /// <summary>Returns the number of different levels in the hierarchy.</summary>
        constexpr size_t level_count() const noexcept { return _levels.size(); }

    private:

        std::vector<std::vector<Ty>> _levels;
    };


    struct HierarchyNode final
    {
        // Index of the parent node.
        uint32_t parent_index;

        // Index of the next sibling node.
        uint32_t sibling_index;

        // Index of the first child node.
        uint32_t child_index;
    };
}

#endif // !DRAKO_GUI_HIERARCHY_HPP
