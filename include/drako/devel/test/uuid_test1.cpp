#include "drako/devel/assertion.hpp"
#include "drako/devel/uuid.hpp"

#include <variant>

int main()
{
    using namespace drako;
    {
        const uuid a;
        const uuid b;

        DRAKO_ASSERT(!a.has_value());
        DRAKO_ASSERT(!b.has_value());

        DRAKO_ASSERT(a == b);
        DRAKO_ASSERT(!(a != b));

        DRAKO_ASSERT(a >= b);
        DRAKO_ASSERT(b >= a);

        DRAKO_ASSERT(!(a > b));
        DRAKO_ASSERT(!(b > a));
    }
    {
        const auto parse1 = parse("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
        DRAKO_ASSERT(std::holds_alternative<uuid>(parse1));
        const auto a = std::get<uuid>(parse1);

        const auto parse2 = parse("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
        DRAKO_ASSERT(std::holds_alternative<uuid>(parse2));
        const auto b = std::get<uuid>(parse2);

        DRAKO_ASSERT(a.has_value());
        DRAKO_ASSERT(b.has_value());

        DRAKO_ASSERT(a == b);
        DRAKO_ASSERT(!(a != b));
    }
    {
        const uuid a = make_uuid_version1();
        const uuid b = make_uuid_version1();

        DRAKO_ASSERT(a.has_value());
        DRAKO_ASSERT(b.has_value());

        DRAKO_ASSERT(a != b);
        DRAKO_ASSERT(!(a == b));

        DRAKO_ASSERT(a < b);
        DRAKO_ASSERT(!(a > b));
    }
}