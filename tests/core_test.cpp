// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "core/base64.hpp"
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
    std::string name;
    uint32_t materialIndex;

    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.property(name, "name");
        archive.property(materialIndex, "materialIndex");
    }
};

struct ShaderData
{
    uint32_t shaderInt;
    float shaderFloat;
    std::string name;
    std::vector<std::string> names;
    std::unordered_map<std::string, std::string> mapNames;
    bool shaderBool;
    InternalData internalData;
    TestEnum testEnum;
    std::unique_ptr<InternalData> internalData2;
    std::array<int32_t, 2> positions;
    std::unordered_map<TestEnum, std::string> enumNames;
    std::unordered_map<uint32_t, std::string> valueNames;
    std::optional<uint32_t> optionalInt;
    std::optional<float> optionalFloat;

    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.property(shaderInt, "shaderInt");
        archive.property(shaderFloat, "shaderFloat");
        archive.property(name, "name");
        archive.property(names, "names");
        archive.property(mapNames, "mapNames");
        archive.property(shaderBool, "shaderBool");
        archive.property(internalData, "internal");
        archive.property(testEnum, "testEnum");
        archive.property(internalData2, "internal2");
        archive.property(positions, "positions");
        archive.property(enumNames, "enumNames");
        archive.property(valueNames, "enumNames");
        archive.property(optionalInt, "optionalInt");
        archive.property(optionalFloat, "optionalFloat");
    }
};

struct ShaderFile
{
    uint32_t magic;
    float shaderFloat;
    std::string name;
    std::vector<std::string> names;
    std::array<uint32_t, 3> numArray;
    ShaderData shaderData;

    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.property(magic);
        archive.property(shaderFloat);
        archive.property(name);
        archive.property(names);
        archive.property(numArray);
        archive.template with<core::serialize_ojson, core::serialize_ijson>(shaderData);
    }
};

TEST(Core, Serialize_JSON_Test)
{
    auto internalData = std::make_unique<InternalData>();
    internalData->name = "bye!";
    internalData->materialIndex = 3;

    ShaderData shaderData{.shaderInt = 4,
                          .shaderFloat = 1.1f,
                          .name = "Hello world!",
                          .names = {"name1", "name2"},
                          .mapNames = {{"keyName1", "keyValue1"}, {"keyName2", "keyValue2"}},
                          .shaderBool = true,
                          .internalData = {"hello!", 2},
                          .testEnum = TestEnum::Second,
                          .internalData2 = std::move(internalData),
                          .positions = {20, 30},
                          .enumNames = {{TestEnum::First, "firstValue"}, {TestEnum::Second, "secondValue"}},
                          .valueNames = {{32, "firstValue"}, {33, "secondValue"}},
                          .optionalInt = 2};

    auto buffer = core::serialize<ShaderData, core::serialize_ojson, std::basic_stringstream<uint8_t>>(shaderData).value();
    auto object = core::deserialize<ShaderData, core::serialize_ijson>(buffer).value();

    ASSERT_EQ(object.shaderInt, shaderData.shaderInt);
    ASSERT_EQ(object.shaderFloat, shaderData.shaderFloat);
    ASSERT_EQ(object.name, shaderData.name);
    ASSERT_EQ(object.names, shaderData.names);
    ASSERT_EQ(object.mapNames, shaderData.mapNames);
    ASSERT_EQ(object.shaderBool, shaderData.shaderBool);
    ASSERT_EQ(object.internalData.name, shaderData.internalData.name);
    ASSERT_EQ(object.internalData.materialIndex, shaderData.internalData.materialIndex);
    ASSERT_EQ(object.testEnum, shaderData.testEnum);
    ASSERT_EQ(object.internalData2->name, shaderData.internalData2->name);
    ASSERT_EQ(object.internalData2->materialIndex, shaderData.internalData2->materialIndex);
    ASSERT_EQ(object.positions, shaderData.positions);
    ASSERT_EQ(object.enumNames, shaderData.enumNames);
    ASSERT_EQ(object.valueNames, shaderData.valueNames);
    ASSERT_EQ(object.optionalInt, shaderData.optionalInt);
    ASSERT_EQ(object.optionalFloat, shaderData.optionalFloat);
}

TEST(Core, Serialize_Archive_Test)
{
    auto internalData = std::make_unique<InternalData>();
    internalData->name = "bye!";
    internalData->materialIndex = 3;

    ShaderData shaderData{4,
                          1.1f,
                          "Hello world!",
                          {"name1", "name2"},
                          {{"keyName1", "keyValue1"}, {"keyName2", "keyValue2"}},
                          true,
                          {"hello!", 2},
                          TestEnum::Second,
                          std::move(internalData),
                          {20, 30}};

    ShaderFile shaderFile{.magic = 2,
                          .shaderFloat = 1.2f,
                          .name = "Hello world!",
                          .names = {"name1", "name2"},
                          .numArray = {5, 3, 4},
                          .shaderData = std::move(shaderData)};

    auto buffer = core::serialize<ShaderFile, core::serialize_oarchive, std::basic_stringstream<uint8_t>>(shaderFile).value();
    auto object = core::deserialize<ShaderFile, core::serialize_iarchive>(buffer).value();

    ASSERT_EQ(object.magic, shaderFile.magic);
    ASSERT_EQ(object.shaderFloat, shaderFile.shaderFloat);
    ASSERT_EQ(object.name, shaderFile.name);
    ASSERT_EQ(object.names, shaderFile.names);
    ASSERT_EQ(object.numArray, shaderFile.numArray);

    // with object
    ASSERT_EQ(object.shaderData.shaderInt, shaderFile.shaderData.shaderInt);
    ASSERT_EQ(object.shaderData.shaderFloat, shaderFile.shaderData.shaderFloat);
    ASSERT_EQ(object.shaderData.name, shaderFile.shaderData.name);
    ASSERT_EQ(object.shaderData.names, shaderFile.shaderData.names);
    ASSERT_EQ(object.shaderData.mapNames, shaderFile.shaderData.mapNames);
    ASSERT_EQ(object.shaderData.shaderBool, shaderFile.shaderData.shaderBool);
    ASSERT_EQ(object.shaderData.internalData.name, shaderFile.shaderData.internalData.name);
    ASSERT_EQ(object.shaderData.internalData.materialIndex, shaderFile.shaderData.internalData.materialIndex);
    ASSERT_EQ(object.shaderData.testEnum, shaderFile.shaderData.testEnum);
    ASSERT_EQ(object.shaderData.internalData2->name, shaderFile.shaderData.internalData2->name);
    ASSERT_EQ(object.shaderData.internalData2->materialIndex, shaderFile.shaderData.internalData2->materialIndex);
    ASSERT_EQ(object.shaderData.positions, shaderFile.shaderData.positions);
}

TEST(Core, Base64_Encode)
{
    std::string test = "Hello world!";
    std::string encodedString = core::base64_encode(std::span<uint8_t const>((uint8_t const*)test.data(), test.size()));
    ASSERT_EQ(encodedString, "SGVsbG8gd29ybGQh");
}

TEST(Core, Base64_Decode)
{
    std::string test = "SGVsbG8gd29ybGQh";
    auto buffer = core::base64_decode(test).value();
    ASSERT_EQ(std::string(reinterpret_cast<char*>(buffer.data()), buffer.size()), "Hello world!");
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}