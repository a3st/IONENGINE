// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

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
struct core::SerializableEnum<TestEnum>
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
        archive.template with<core::OutputJSON, core::InputJSON>(shaderData);
    }
};

TEST(Serialize, JSON_Test)
{
    auto internalData = std::make_unique<InternalData>();
    internalData->name = "bye!";
    internalData->materialIndex = 3;

    ShaderData shaderData = {.shaderInt = 4,
                             .shaderFloat = 1.1f,
                             .name = "Hello world!",
                             .names = {"name1", "name2"},
                             .mapNames = {{"keyName1", "keyValue1"}, {"keyName2", "keyValue2"}},
                             .shaderBool = true,
                             .internalData = {"hello!", 2},
                             .testEnum = TestEnum::Second,
                             .internalData2 = std::move(internalData),
                             .positions = {20, 30},
                             .enumNames = {{TestEnum::First, "firstValue"}, {TestEnum::Second, "secondValue"}}};

    auto result = core::saveToBytes<ShaderData, core::OutputJSON>(shaderData);
    auto buffer = std::move(result.value());

    auto resultAfter = core::loadFromBytes<ShaderData, core::InputJSON>(buffer);
    auto object = std::move(resultAfter.value());

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
}

TEST(Serialize, Archive_Test)
{
    auto internalData = std::make_unique<InternalData>();
    internalData->name = "bye!";
    internalData->materialIndex = 3;

    ShaderData shaderData = {4,
                             1.1f,
                             "Hello world!",
                             {"name1", "name2"},
                             {{"keyName1", "keyValue1"}, {"keyName2", "keyValue2"}},
                             true,
                             {"hello!", 2},
                             TestEnum::Second,
                             std::move(internalData),
                             {20, 30}};

    ShaderFile shaderFile = {.magic = 2,
                             .shaderFloat = 1.2f,
                             .name = "Hello world!",
                             .names = {"name1", "name2"},
                             .numArray = {5, 3, 4},
                             .shaderData = std::move(shaderData)};

    auto result = core::saveToBytes<ShaderFile, core::OutputArchive>(shaderFile);
    auto buffer = std::move(result.value());

    auto resultAfter = core::loadFromBytes<ShaderFile, core::InputArchive>(buffer);
    auto object = std::move(resultAfter.value());

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

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}