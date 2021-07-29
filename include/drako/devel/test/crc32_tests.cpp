#include "drako/devel/crc.hpp"

#include <gtest/gtest.h>

#include <string>

using namespace drako;

GTEST_TEST(Crc32c, UnalignedBuffer)
{
    // test samples validated with http://www.sunshine2k.de/coding/javascript/crc/crc_js.html

    /*
    const std::tuple<std::string, std::uint32_t> samples[] = {
        { "W", 0xD792ED69 },
        { "WA", 0xF8CE17AD },
        { "Weeks of programming can save you hours of planning", 0xE94A5429 },
        { "The computer was born to solve problems that did not exist before", 0xBCE523F4 },
        { "One bad programmer can easily create two new jobs a year", 0xb16b3cce }
    };
    */

    //const std::string c = "Weeks of programming can save you hours of planning";
    //EXPECT_EQ(drako::crc32c(c), 0xE94A5429);

    EXPECT_EQ(crc32c("WA"), 0xF8CE17AD);
    EXPECT_EQ(crc32c("dead"), 0x671AC5FD);
    EXPECT_EQ(crc32c("deadbeef"), 0xDCCF39C7);
    EXPECT_EQ(crc32c("deadbeefdeadbeef"), 0x8809AA9A);
    EXPECT_EQ(crc32c("I tried so hard"), 0x205DD030);
    EXPECT_EQ(crc32c("Weeks of programming can save you hours of planning"), 0x5757814D);
    EXPECT_EQ(crc32c("The computer was born to solve problems that did not exist before"), 0x2F7D02E5);

    //for (const auto& [s, expected] : samples)
    //    EXPECT_EQ(drako::crc32c(s), expected);
}

/*
GTEST_TEST(Crc32c, Aligned32Bit)
{
    const std::uint32_t bytes[] = { 0x00112233, 0x11223344, 0x22334455 };
    EXPECT_EQ(drako::crc32c(bytes), 0xAA7193F6);
}

GTEST_TEST(Crc32c, Aligned64Bit)
{
    const std::uint64_t bytes[] = { 0x00112233 };
    EXPECT_EQ(drako::crc32c(bytes), 0xE6F9CD9A);
}
*/