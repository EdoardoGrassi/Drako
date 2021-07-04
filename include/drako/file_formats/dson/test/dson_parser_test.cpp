#include "drako/file_formats/dson/dson.hpp"

#include "gtest/gtest.h"

#include <iostream>
#include <string>

using namespace drako;

GTEST_TEST(Parser, KeyValuePairs)
{
    const char text[] = "key1=val1\n"
                        "key2=val2\n"
                        "key3=val3\n"
                        "key4=val4\n";
    const auto dom = dson::parse(text);
    for (const auto [k, v] : dom)
        std::cout << k << " = " << v << '\n';

    ASSERT_EQ(dom.get("key1"), "val1");
    ASSERT_EQ(dom.get("key2"), "val2");
    ASSERT_EQ(dom.get("key3"), "val3");
    ASSERT_EQ(dom.get("key4"), "val4");
}

GTEST_TEST(Parser, SerializationAndDeserialization)
{
    const auto SAMPLES = 100u;

    dson::DOM dom{};
    for (auto i = 0; i < SAMPLES; ++i)
        dom.set("key" + std::to_string(i), "val" + std::to_string(i));

    for (auto i = 0; i < SAMPLES; ++i)
        ASSERT_EQ(dom.get("key" + std::to_string(i)), ("val" + std::to_string(i)));
}