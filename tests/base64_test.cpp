#include "core/base64.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(Base64, Encode)
{
    std::string test = "Hello world!";
    std::string encodedString =
        core::base64::encode(std::span<uint8_t const>((uint8_t const*)test.data(), test.size()));
    ASSERT_EQ(encodedString, "SGVsbG8gd29ybGQh");
}

TEST(Base64, Decode)
{
    std::string test = "SGVsbG8gd29ybGQh";
    auto buffer = core::base64::decode(test).value();
    ASSERT_EQ(std::string(reinterpret_cast<char*>(buffer.data()), buffer.size()), "Hello world!");
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}