#include "core/serializable.hpp"
#include "precompiled.h"

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
    uint32_t shader;
    float shaderFloat;
    std::string name;
    std::vector<std::string> names;
    std::unordered_map<std::string, std::string> mapNames;
    bool shaderBool;
    InternalData internalData;
    TestEnum testEnum;
    std::unique_ptr<InternalData> internalData2;
    std::array<int32_t, 2> positions;

    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.property(shader, "shader");
        archive.property(shaderFloat, "shaderFloat");
        archive.property(name, "name");
        archive.property(names, "names");
        archive.property(mapNames, "mapNames");
        archive.property(shaderBool, "shaderBool");
        archive.property(internalData, "internal");
        archive.property(testEnum, "testEnum");
        archive.property(internalData2, "internal2");
        archive.property(positions, "positions");
    }
};

struct ShaderFile
{
    uint32_t magic;
    ShaderData shaderData;

    template <typename Archive>
    auto operator()(Archive& archive)
    {
        archive.property(magic);
        archive.with<core::serialize::OutputJSON, core::serialize::InputJSON>(shaderData);
    }
};

auto main(int32_t argc, char** argv) -> int32_t
{
    auto internal2 = std::make_unique<InternalData>();
    internal2->name = "bye!";
    internal2->materialIndex = 3;

    ShaderData data = {4,
                       1.1f,
                       "Hello world!",
                       {"name1", "name2"},
                       {{"keyName1", "keyValue1"}, {"keyName2", "keyValue2"}},
                       true,
                       {"hello!", 2},
                       TestEnum::Second,
                       std::move(internal2),
                       {20, 30}};

    auto result = core::saveToBytes<ShaderData, core::serialize::OutputJSON>(data);
    auto buffer = result.value();
    std::cout << "Input: " << std::string(reinterpret_cast<char*>(buffer.data()), buffer.size()) << std::endl;

    auto resultAfter = core::loadFromBytes<ShaderData, core::serialize::InputJSON>(buffer);
    auto object = std::move(resultAfter.value());

    std::cout << object.shader << std::endl;
    std::cout << object.shaderFloat << std::endl;
    std::cout << object.name << std::endl;

    for (auto const& e : object.names)
    {
        std::cout << e << std::endl;
    }

    for (auto const& [key, value] : object.mapNames)
    {
        std::cout << key << " : " << value << std::endl;
    }

    std::cout << std::boolalpha << object.shaderBool << std::endl;

    std::cout << object.internalData.name << std::endl;
    std::cout << object.internalData.materialIndex << std::endl;
    std::cout << static_cast<uint32_t>(object.testEnum) << std::endl;

    std::cout << object.internalData2->name << std::endl;
    std::cout << object.internalData2->materialIndex << std::endl;

    for (auto const& e : object.positions)
    {
        std::cout << e << std::endl;
    }

    ShaderFile shaderFile = {
        .magic = 0x1,
        .shaderData = std::move(data)
    };

    auto result5 = core::saveToFile<ShaderFile>(shaderFile, "testBinary.bin");
    
    return 0;
}