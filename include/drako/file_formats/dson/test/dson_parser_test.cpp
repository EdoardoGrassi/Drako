#include "drako/file_formats/dson/dson.hpp"

#include <cassert>
#include <iostream>
#include <string>

int main()
{
    using namespace drako;

    {
        const char text[] = "key1=val1\n"
                            "key2=val2\n"
                            "key3=val3\n"
                            "key4=val4\n";
        auto dom = dson::parse(text);
        for (const auto [k, v] : dom)
            std::cout << k << " = " << v << '\n';
    }
    {
        const auto SAMPLES = 1000u;

        dson::DOM dom{};
        for (auto i = 0; i < SAMPLES; ++i)
            dom.set("key" + std::to_string(i), "val" + std::to_string(i));

        try
        {
            for (auto i = 0; i < SAMPLES; ++i)
                assert(dom.get("key" + std::to_string(i)) == ("val" + std::to_string(i)));
        }
        catch (...)
        {
            assert(false);
        }
    }
}