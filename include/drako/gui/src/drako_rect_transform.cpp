#include "drako_rect_transform.hpp"

namespace Drako
{
    DkRect::DkRect(DkPoint topLeftCorner, DkPoint bottomRightCorner)
        : m_topLeftCorner(topLeftCorner), m_bottomRightCorner(bottomRightCorner)
    {
    }

    DkRect::DkRect(DkPoint topLeftCorner, float width, float height)
        : m_topLeftCorner(topLeftCorner), m_bottomRightCorner(topLeftCorner + DkPoint(width, height))
    {
    }
}