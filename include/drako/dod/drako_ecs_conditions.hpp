#ifndef DRAKO_ECS_CONDITIONS_HPP_
#define DRAKO_ECS_CONDITIONS_HPP_

#include <cstdlib>
#include <functional>

#include "drako_ecs_table.hpp"

namespace drako::dod
{
    template <typename TBitField>
    class ConditionalQuery
    {
    public:

        explicit constexpr ConditionalQuery(const DataTable<TBitField>& pTable,
                                            const std::function<bool(TBitField)>& condition)
            : m_size(size)
        {

        }

        inline constexpr ConditionalQuery& operator^(const ConditionalQuery& rhs);

    private:

        size_t m_size;
        bool* m_p;
    };


    template <size_t BitfieldSize,
        typename TBitField,
        std::enable_if_t<((sizeof(TBitField) * 8) >= BitfieldSize)> = false>
    class ConditionalTable
    {
    public:

        explicit ConditionalTable(const size_t size) : m_size(size)
        {
            m_pInputData = static_cast<TBitField*>(std::malloc(sizeof(TBitField) * size));
        }
        ~ConditionalTable() noexcept
        {
            std::free(m_pInputData);
        }

        inline constexpr const TBitField operator[](size_t idx) const noexcept { return m_pInputData[idx]; }
        inline constexpr TBitField operator[](size_t idx) noexcept { return m_pInputData[idx]; }

        inline constexpr void SetCell(size_t rowIdx, size_t columnIdx, bool value) noexcept
        {
            //m_pInputData[rowIdx] |= static_cast<TBitField>(value) << predicateIdx;
        }
        inline constexpr bool GetCell(size_t rowIdx, size_t columnIdx) const noexcept
        {

        }

        inline constexpr static TBitField CreateMask() const noexcept;

    private:

        size_t m_size;
        TBitField* m_pInputData;
    };

    template <>
    class ConditionalTable<BitfieldSize, uint8_t> {};

    template <>
    class ConditionalTable<BitfieldSize, uint16_t> {};

    template <>
    class ConditionalTable<BitfieldSize, uint32_t> {};


    template <typename T>
    struct BitMask
    {
        explicit constexpr BitMask() noexcept;
    };
}

#endif // !DRAKO_ECS_CONDITIONS_HPP_
