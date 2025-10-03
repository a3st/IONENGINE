// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "core/base64.hpp"
#include "core/event.hpp"
#include "core/serialize.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

enum class TestEnum
{
    First,
    Second,
    Third
};

template <>
struct core::serializable_enum<TestEnum>
{
    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.field(TestEnum::First, "first");
        archive.field(TestEnum::Second, "second");
        archive.field(TestEnum::Third, "third");
    }
};

struct InternalData
{
    std::string testString;
    uint32_t testInt;

    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.property(testString, "testString");
        archive.property(testInt, "testInt");
    }
};

struct Data
{
    uint32_t testInt;
    float testFloat;
    std::string testString;
    std::vector<std::string> testStrings;
    std::unordered_map<std::string, std::string> testMapStrings;
    bool testBool;
    InternalData internalData;
    TestEnum testEnum;
    std::unique_ptr<InternalData> internalDataPtr;
    std::array<int32_t, 2> testInts;
    std::unordered_map<TestEnum, std::string> testMapEnums;
    std::unordered_map<uint32_t, std::string> testMapInts;
    std::optional<uint32_t> testOptInt;
    std::optional<float> testOptFloat;

    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.property(testInt, "testInt");
        archive.property(testFloat, "testFloat");
        archive.property(testString, "testString");
        archive.property(testStrings, "testStrings");
        archive.property(testMapStrings, "testMapStrings");
        archive.property(testBool, "testBool");
        archive.property(internalData, "internalData");
        archive.property(testEnum, "testEnum");
        archive.property(internalDataPtr, "internalDataPtr");
        archive.property(testInts, "testInts");
        archive.property(testMapEnums, "testMapEnums");
        archive.property(testMapInts, "testMapInts");
        archive.property(testOptInt, "testOptInt");
        archive.property(testOptFloat, "testOptFloat");
    }
};

struct DataFile
{
    uint32_t magic;
    float testFloat;
    std::string testString;
    std::vector<std::string> testStrings;
    std::array<uint32_t, 3> testInts;
    Data data;

    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.property(magic);
        archive.property(testFloat);
        archive.property(testString);
        archive.property(testStrings);
        archive.property(testInts);
        archive.template with<core::serialize_ojson, core::serialize_ijson>(data);
    }
};

TEST(Core, Serialize_JSON_Test)
{
    auto internalData = std::make_unique<InternalData>();
    internalData->testString = "Hello world!";
    internalData->testInt = 3;

    Data data{.testInt = 4,
              .testFloat = 1.1f,
              .testString = "Hello world!",
              .testStrings = {"string1", "string2"},
              .testMapStrings = {{"key1", "value1"}, {"key2", "value2"}},
              .testBool = true,
              .internalData = {"Hello world!", 2},
              .testEnum = TestEnum::Second,
              .internalDataPtr = std::move(internalData),
              .testInts = {20, 30},
              .testMapEnums = {{TestEnum::First, "first"}, {TestEnum::Second, "second"}},
              .testMapInts = {{32, "first"}, {33, "second"}},
              .testOptInt = 2};

    auto bufferObject = core::serialize<core::serialize_ojson, std::basic_stringstream<uint8_t>>(data).value();
    auto deserializedObject = core::deserialize<core::serialize_ijson, Data>(bufferObject).value();

    ASSERT_EQ(deserializedObject.testInt, data.testInt);
    ASSERT_EQ(deserializedObject.testFloat, data.testFloat);
    ASSERT_STREQ(deserializedObject.testString.c_str(), data.testString.c_str());
    ASSERT_EQ(deserializedObject.testStrings, data.testStrings);
    ASSERT_EQ(deserializedObject.testMapStrings, data.testMapStrings);
    ASSERT_EQ(deserializedObject.testBool, data.testBool);

    ASSERT_EQ(deserializedObject.internalData.testString, data.internalData.testString);
    ASSERT_EQ(deserializedObject.internalData.testInt, data.internalData.testInt);

    ASSERT_EQ(deserializedObject.testEnum, data.testEnum);

    ASSERT_STREQ(deserializedObject.internalDataPtr->testString.c_str(), data.internalDataPtr->testString.c_str());
    ASSERT_EQ(deserializedObject.internalDataPtr->testInt, data.internalDataPtr->testInt);

    ASSERT_EQ(deserializedObject.testInts, data.testInts);
    ASSERT_EQ(deserializedObject.testMapEnums, data.testMapEnums);
    ASSERT_EQ(deserializedObject.testMapInts, data.testMapInts);
    ASSERT_EQ(deserializedObject.testOptInt, data.testOptInt);
    ASSERT_EQ(deserializedObject.testOptFloat, data.testOptFloat);
}

TEST(Core, Serialize_Archive_Test)
{
    auto internalData = std::make_unique<InternalData>();
    internalData->testString = "Hello world!";
    internalData->testInt = 3;

    Data data{.testInt = 4,
              .testFloat = 1.1f,
              .testString = "Hello world!",
              .testStrings = {"string1", "string2"},
              .testMapStrings = {{"key1", "value1"}, {"key2", "value2"}},
              .testBool = true,
              .internalData = {"Hello world!", 2},
              .testEnum = TestEnum::Second,
              .internalDataPtr = std::move(internalData),
              .testInts = {20, 30},
              .testMapEnums = {{TestEnum::First, "first"}, {TestEnum::Second, "second"}},
              .testMapInts = {{32, "first"}, {33, "second"}},
              .testOptInt = 2};

    DataFile dataFile{.magic = 2,
                      .testFloat = 1.2f,
                      .testString = "Hello world!",
                      .testStrings = {"string1", "string2"},
                      .testInts = {5, 3, 4},
                      .data = std::move(data)};

    auto bufferObject = core::serialize<core::serialize_oarchive, std::basic_stringstream<uint8_t>>(dataFile).value();
    auto deserializedObject = core::deserialize<core::serialize_iarchive, DataFile>(bufferObject).value();

    ASSERT_EQ(deserializedObject.magic, dataFile.magic);
    ASSERT_EQ(deserializedObject.testFloat, dataFile.testFloat);
    ASSERT_EQ(deserializedObject.testString, dataFile.testString);
    ASSERT_EQ(deserializedObject.testStrings, dataFile.testStrings);
    ASSERT_EQ(deserializedObject.testInts, dataFile.testInts);

    // with object
    ASSERT_EQ(deserializedObject.data.testInt, dataFile.data.testInt);
    ASSERT_EQ(deserializedObject.data.testFloat, dataFile.data.testFloat);
    ASSERT_STREQ(deserializedObject.testString.c_str(), dataFile.data.testString.c_str());
    ASSERT_EQ(deserializedObject.data.testStrings, dataFile.data.testStrings);
    ASSERT_EQ(deserializedObject.data.testMapStrings, dataFile.data.testMapStrings);
    ASSERT_EQ(deserializedObject.data.testBool, dataFile.data.testBool);

    ASSERT_EQ(deserializedObject.data.internalData.testString, dataFile.data.internalData.testString);
    ASSERT_EQ(deserializedObject.data.internalData.testInt, dataFile.data.internalData.testInt);

    ASSERT_EQ(deserializedObject.data.testEnum, dataFile.data.testEnum);

    ASSERT_STREQ(deserializedObject.data.internalDataPtr->testString.c_str(),
                 dataFile.data.internalDataPtr->testString.c_str());
    ASSERT_EQ(deserializedObject.data.internalDataPtr->testInt, dataFile.data.internalDataPtr->testInt);

    ASSERT_EQ(deserializedObject.data.testInts, dataFile.data.testInts);
    ASSERT_EQ(deserializedObject.data.testMapEnums, dataFile.data.testMapEnums);
    ASSERT_EQ(deserializedObject.data.testMapInts, dataFile.data.testMapInts);
    ASSERT_EQ(deserializedObject.data.testOptInt, dataFile.data.testOptInt);
    ASSERT_EQ(deserializedObject.data.testOptFloat, dataFile.data.testOptFloat);
}

TEST(Core, Serialize_Enum_Test)
{
    auto buffer = core::serialize<core::serialize_oenum, std::ostringstream>(TestEnum::Second).value();
    auto object = core::deserialize<core::serialize_ienum, TestEnum>(std::istringstream("third")).value();
}

TEST(Core, Base64_Encode_Test)
{
    std::string testString = "Hello world!";
    std::string encodedString = core::Base64().encode(
        std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(testString.data()), testString.size()));
    ASSERT_STREQ(encodedString.c_str(), "SGVsbG8gd29ybGQh");
}

TEST(Core, Base64_Decode_Test)
{
    std::string testString = "SGVsbG8gd29ybGQh";
    auto decodedBuffer = core::Base64().decode(testString).value();
    std::string decodedString(reinterpret_cast<char const*>(decodedBuffer.data()), decodedBuffer.size());
    ASSERT_STREQ(decodedString.c_str(), "Hello world!");
}

TEST(Core, Event_Test)
{
    int32_t const testValue = 5;
    core::event<void(int32_t)> testEvent;

    testEvent += [&](int32_t a) -> void { ASSERT_EQ(a, testValue); };
    testEvent += [&](int32_t a) -> void { ASSERT_EQ(a + 2, testValue + 2); };
    testEvent += [&](int32_t a) -> void { ASSERT_EQ(a + 3, testValue + 3); };

    testEvent(testValue);
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}