#ifndef DRAKO_ECS_FUNCTION_ARRAY_HPP_
#define DRAKO_ECS_FUNCTION_ARRAY_HPP_

#include <vector>

namespace drako::dod
{
    /// <summary>Function pointers are allocated as a compact table and deferenced directly.</summary>
    struct DirectLookupImpl {};

    template <typename TFunction, typename TImplementationPolicy>
    class FunctionArray
    {};

    template <typename TFunction>
    class FunctionArray<TFunction, DirectLookupImpl>
    {
    public:

        constexpr explicit FunctionArray(const std::size_t allocatedSlots) {}

        inline constexpr operator[](const std::size_t idx) const noexcept { return m_functions[idx]; }

        inline constexpr void AddComponent(TFunction pFunction) {}

    private:

        std::vector<TFunction> m_functions;
    };


    /// <summary>Function pointers are allocated as a separate table and indexed through an offset.</summary>
    struct IndexedLookupImpl {};

    template <typename TFunction>
    class FunctionArray<TFunction, IndexedLookupImpl>
    {
    public:

        constexpr explicit FunctionArray(const std::size_t allocatedSlots) {}

        inline constexpr operator[](const std::size_t idx) const noexcept { return m_functions[m_indexes[idx]]; }

        inline constexpr void AddComponent(TFunction pFunction) {}

    private:

        std::vector<TFunction> m_functions;
        std::vector<std::uint32_t> m_indexes;
    };
}

#endif // !DRAKO_ECS_FUNCTION_ARRAY_HPP_
