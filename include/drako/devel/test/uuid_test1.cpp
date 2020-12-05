#include "drako/devel/uuid.hpp"
#include "drako/devel/uuid_engine.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

int main()
{
    using namespace drako;
    {
        const Uuid a{};
        const Uuid b{};

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
        Uuid a, b;
        parse("6ba7b810-9dad-11d1-80b4-00c04fd430c8", a);
        parse("6ba7b810-9dad-11d1-80b4-00c04fd430c8", b);

        assert(a.has_value());
        assert(b.has_value());

        assert(a == b);
        assert(!(a != b));
    }
    {
        UuidMacEngine gen{};
        const Uuid    a = gen();
        const Uuid    b = gen();

        assert(a.has_value());
        assert(b.has_value());

        assert(a != b);
        assert(!(a == b));

        assert(a < b);
        assert(!(a > b));
    }
    {
        const auto iters = 1000u;

        std::vector<Uuid> v;
        v.reserve(iters);

        UuidMacEngine gen{};
        for (auto i = 0; i < iters; ++i)
        {
            const auto u = gen();
            assert(std::find(std::cbegin(v), std::cend(v), u) == std::cend(v));
            v.push_back(u);
        }
    }
}