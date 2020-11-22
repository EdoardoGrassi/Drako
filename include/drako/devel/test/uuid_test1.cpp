#include "drako/devel/uuid.hpp"
#include "drako/devel/uuid_engine.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

int main()
{
    using namespace drako;
    {
        const uuid a{};
        const uuid b{};

        assert(!a.has_value());
        assert(!b.has_value());

        assert(a == b);
        assert(!(a != b));

        assert(a >= b);
        assert(b >= a);

        assert(!(a > b));
        assert(!(b > a));
    }
    {
        const auto a = uuid::parse("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
        const auto b = uuid::parse("6ba7b810-9dad-11d1-80b4-00c04fd430c8");

        assert(a.has_value());
        assert(b.has_value());

        assert(a == b);
        assert(!(a != b));
    }
    {
        uuid_v1_engine gen{};
        const uuid     a = gen();
        const uuid     b = gen();

        assert(a.has_value());
        assert(b.has_value());

        assert(a != b);
        assert(!(a == b));

        assert(a < b);
        assert(!(a > b));
    }
    {
        const auto iters = 1000u;

        std::vector<uuid> v;
        v.reserve(iters);

        uuid_v1_engine gen{};
        for (auto i = 0; i < iters; ++i)
        {
            const auto u = gen();
            assert(std::find(std::cbegin(v), std::cend(v), u) == std::cend(v));
            v.push_back(u);
        }
    }
}