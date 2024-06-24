// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/exception.hpp"
#include <simdjson.h>

namespace ionengine::core
{
    template <typename Type>
    struct SerializableEnum
    {
        template <typename Archive>
        auto operator()(Archive& archive);
    };

    namespace serialize
    {
        class InputJSON;
        class OutputJSON;
        class InputArchive;
        class OutputArchive;

        namespace internal
        {
            template <typename>
            struct is_std_vector : std::false_type
            {
            };

            template <typename T, typename A>
            struct is_std_vector<std::vector<T, A>> : std::true_type
            {
            };

            template <typename>
            struct is_std_unique_ptr : std::false_type
            {
            };

            template <typename T, typename A>
            struct is_std_unique_ptr<std::unique_ptr<T, A>> : std::true_type
            {
            };

            template <typename>
            struct is_std_array : std::false_type
            {
            };

            template <typename T, std::size_t N>
            struct is_std_array<std::array<T, N>> : std::true_type
            {
            };

            template <typename Type>
            auto propertyResolveFromJSON(InputJSON& inputJSON, simdjson::ondemand::value it, Type& element) -> void;

            template <typename Type>
            auto propertyResolveToJSON(OutputJSON& outputJSON, std::string_view const jsonName,
                                       Type const& element) -> void;

            template <typename Type>
            auto propertyResolveToBinary(OutputArchive outputArchive, Type const& element) -> void;
        } // namespace internal

        class InputJSON
        {
            template <typename Type>
            friend auto internal::propertyResolveFromJSON(InputJSON& inputJSON, simdjson::ondemand::value it,
                                                          Type& element) -> void;

          public:
            InputJSON(std::basic_istream<uint8_t>& stream) : stream(&stream)
            {
                jsonData = std::string(std::istreambuf_iterator<uint8_t>(stream.rdbuf()), {});
                document = parser.iterate(jsonData, jsonData.size() + simdjson::SIMDJSON_PADDING);
            }

            template <typename Type>
            auto property(Type& element, std::string_view const jsonName) -> void
            {
                internal::propertyResolveFromJSON(*this, document[jsonName], element);
            }

            template <typename Type>
            auto field(Type const& element, std::string_view const jsonName) -> void
            {
                enumFields[std::string(jsonName)] = static_cast<uint32_t>(element);
            }

            template <typename Type>
            auto operator()(Type& object) -> size_t
            {
                object(*this);
                return stream->tellg();
            }

          private:
            std::string jsonData;
            simdjson::ondemand::parser parser;
            simdjson::ondemand::document_stream::iterator::value_type document;
            std::basic_istream<uint8_t>* stream;
            std::unordered_map<std::string, uint32_t> enumFields;
        };

        namespace internal
        {
            template <typename Type>
            auto propertyResolveFromJSON(InputJSON& inputJSON, simdjson::ondemand::value it, Type& element) -> void
            {
                if constexpr (std::is_integral_v<Type> && !std::is_same_v<Type, bool>)
                {
                    int64_t value;
                    auto error = it.get_int64().get(value);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    element = value;
                }
                else if constexpr (std::is_integral_v<Type> && std::is_same_v<Type, bool>)
                {
                    bool value;
                    auto error = it.get_bool().get(value);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    element = value;
                }
                else if constexpr (std::is_floating_point_v<Type>)
                {
                    simdjson::ondemand::number value;
                    auto error = it.get_number().get(value);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    element = static_cast<Type>(value.get_double());
                }
                else if constexpr (std::is_same_v<
                                       Type, std::basic_string<char, std::char_traits<char>, std::allocator<char>>>)
                {
                    std::string_view value;
                    auto error = it.get_string().get(value);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    element = std::string(value);
                }
                else if constexpr (std::is_scoped_enum_v<Type>)
                {
                    SerializableEnum<Type> object;
                    object(inputJSON);

                    std::string_view value;
                    auto error = it.get_string().get(value);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    auto result = inputJSON.enumFields.find(std::string(value));
                    if (result == inputJSON.enumFields.end())
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    element = static_cast<Type>(result->second);
                }
                else if constexpr (is_std_vector<Type>::value)
                {
                    simdjson::ondemand::array elements;
                    auto error = it.get_array().get(elements);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    element.resize(elements.count_elements());

                    uint32_t i = 0;
                    for (auto e : elements)
                    {
                        propertyResolveFromJSON(inputJSON, e.value(), element[i]);
                        ++i;
                    }
                }
                else if constexpr (is_std_array<Type>::value)
                {
                    simdjson::ondemand::array elements;
                    auto error = it.get_array().get(elements);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    auto constexpr arraySize = std::tuple_size<Type>::value;

                    uint32_t i = 0;
                    for (auto e : elements)
                    {
                        if (i >= arraySize)
                        {
                            throw core::Exception("An error occurred while deserializing a field");
                        }
                        propertyResolveFromJSON(inputJSON, e.value(), element[i]);
                        ++i;
                    }
                }
                else if constexpr (is_std_unique_ptr<Type>::value)
                {
                    element = std::make_unique<typename Type::element_type>();
                    propertyResolveFromJSON(inputJSON, it, *element);
                }
                else if constexpr (std::is_same_v<
                                       Type,
                                       std::unordered_map<
                                           std::basic_string<char, std::char_traits<char>, std::allocator<char>>,
                                           std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>)
                {
                    simdjson::ondemand::object elements;
                    auto error = it.get_object().get(elements);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    for (auto e : elements)
                    {
                        std::string_view key;
                        error = e.unescaped_key().get(key);
                        if (error != simdjson::SUCCESS)
                        {
                            throw core::Exception("An error occurred while deserializing a field");
                        }

                        std::string_view value;
                        error = e.value().get_string().get(value);
                        if (error != simdjson::SUCCESS)
                        {
                            throw core::Exception("An error occurred while deserializing a field");
                        }

                        element[std::string(key)] = std::string(value);
                    }
                }
                else
                {
                    simdjson::ondemand::object object;
                    auto error = it.get_object().get(object);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    std::string_view value;
                    error = object.raw_json().get(value);
                    if (error != simdjson::SUCCESS)
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }

                    std::basic_spanstream<uint8_t> tempInput(
                        std::span<uint8_t>(reinterpret_cast<uint8_t*>(const_cast<char*>(value.data())), value.size()));
                    InputJSON archive(tempInput);
                    if (!(archive(element) > 0))
                    {
                        throw core::Exception("An error occurred while deserializing a field");
                    }
                }
            }
        } // namespace internal

        class OutputJSON
        {
            template <typename Type>
            friend auto internal::propertyResolveToJSON(OutputJSON& outputJSON, std::string_view const jsonName,
                                                        Type const& element) -> void;

          public:
            OutputJSON(std::basic_ostream<uint8_t>& stream) : stream(&stream)
            {
                jsonChunk << "{";
            }

            template <typename Type>
            auto property(Type const& element, std::string_view const jsonName) -> void
            {
                internal::propertyResolveToJSON(*this, jsonName, element);
            }

            template <typename Type>
            auto field(Type const& element, std::string_view const jsonName) -> void
            {
                enumFields[static_cast<uint32_t>(element)] = std::string(jsonName);
            }

            template <typename Type>
            auto operator()(Type const& object) -> size_t
            {
                const_cast<Type&>(object)(*this);

                std::string rawJSON = jsonChunk.str();
                rawJSON = rawJSON.substr(0, rawJSON.size() - 1) + "}";

                stream->write(reinterpret_cast<uint8_t const*>(rawJSON.data()), rawJSON.size());
                return stream->tellp();
            }

          private:
            std::stringstream jsonChunk;
            std::basic_ostream<uint8_t>* stream;
            std::unordered_map<uint32_t, std::string> enumFields;
        };

        namespace internal
        {
            template <typename Type>
            auto propertyResolveToJSON(OutputJSON& outputJSON, std::string_view const jsonName,
                                       Type const& element) -> void
            {
                if constexpr (is_std_vector<Type>::value)
                {
                    outputJSON.jsonChunk << "\"" << jsonName << "\":[";
                    bool isFirst = true;
                    for (auto const& e : element)
                    {
                        if (!isFirst)
                        {
                            outputJSON.jsonChunk << ",";
                        }
                        propertyResolveToJSON(outputJSON, "", e);
                        isFirst = false;
                    }
                    outputJSON.jsonChunk << "],";
                }
                else if constexpr (is_std_array<Type>::value)
                {
                    outputJSON.jsonChunk << "\"" << jsonName << "\":[";
                    bool isFirst = true;

                    auto constexpr arraySize = std::tuple_size<Type>::value;

                    uint32_t i = 0;
                    for (auto const& e : element)
                    {
                        if (i >= arraySize)
                        {
                            throw core::Exception("An error occurred while serializing a field");
                        }

                        if (!isFirst)
                        {
                            outputJSON.jsonChunk << ",";
                        }
                        propertyResolveToJSON(outputJSON, "", e);
                        isFirst = false;
                        ++i;
                    }
                    outputJSON.jsonChunk << "],";
                }
                else if constexpr (is_std_unique_ptr<Type>::value)
                {
                    propertyResolveToJSON(outputJSON, jsonName, *element);
                }
                else if constexpr (std::is_same_v<
                                       Type,
                                       std::unordered_map<
                                           std::basic_string<char, std::char_traits<char>, std::allocator<char>>,
                                           std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>)
                {
                    outputJSON.jsonChunk << "\"" << jsonName << "\":{";
                    bool isFirst = true;
                    for (auto const& [key, value] : element)
                    {
                        if (!isFirst)
                        {
                            outputJSON.jsonChunk << ",";
                        }
                        outputJSON.jsonChunk << "\"" << key << "\":\"" << value << "\"";
                        isFirst = false;
                    }
                    outputJSON.jsonChunk << "},";
                }
                else
                {
                    if (!jsonName.empty())
                    {
                        outputJSON.jsonChunk << "\"" << jsonName << "\":";
                    }

                    if constexpr (std::is_integral_v<Type> && !std::is_same_v<Type, bool> ||
                                  std::is_floating_point_v<Type>)
                    {
                        outputJSON.jsonChunk << element;
                    }
                    else if constexpr (std::is_integral_v<Type> && std::is_same_v<Type, bool>)
                    {
                        outputJSON.jsonChunk << std::boolalpha << element;
                    }
                    else if constexpr (std::is_same_v<
                                           Type, std::basic_string<char, std::char_traits<char>, std::allocator<char>>>)
                    {
                        outputJSON.jsonChunk << "\"" << element << "\"";
                    }
                    else if constexpr (std::is_scoped_enum_v<Type>)
                    {
                        SerializableEnum<Type> object;
                        object(outputJSON);

                        auto result = outputJSON.enumFields.find(static_cast<uint32_t>(element));
                        if (result == outputJSON.enumFields.end())
                        {
                            throw core::Exception("An error occurred while serializing a field");
                        }

                        outputJSON.jsonChunk << "\"" << result->second << "\"";
                    }
                    else
                    {
                        std::basic_stringstream<uint8_t> tempOutput;
                        OutputJSON archive(tempOutput);
                        if (archive(element) > 0)
                        {
                            std::basic_string<uint8_t> const rawJSON = tempOutput.str();
                            outputJSON.jsonChunk
                                << std::string_view(reinterpret_cast<char const*>(rawJSON.data()), rawJSON.size());
                        }
                    }

                    if (!jsonName.empty())
                    {
                        outputJSON.jsonChunk << ",";
                    }
                }
            }
        } // namespace internal

        class InputArchive
        {
          public:
            InputArchive(std::basic_istream<uint8_t>& stream) : stream(&stream)
            {
            }

          private:
            std::basic_istream<uint8_t>* stream;
        };

        class OutputArchive
        {
            template <typename Type>
            friend auto internal::propertyResolveToBinary(OutputArchive outputArchive, Type const& element) -> void;

          public:
            OutputArchive(std::basic_ostream<uint8_t>& stream) : stream(&stream)
            {
            }

            template <typename Type>
            auto property(Type const& element) -> void
            {
                internal::propertyResolveToBinary(*this, element);
            }

            template <typename OutputArchive, typename InputArchive, typename Type>
            auto with(Type const& element) -> void
            {
                std::cout << "with" << std::endl;
            }

            template <typename Type>
            auto operator()(Type const& object) -> size_t
            {
                const_cast<Type&>(object)(*this);

                /*std::string rawJSON = jsonChunk.str();
                rawJSON = rawJSON.substr(0, rawJSON.size() - 1) + "}";

                stream->write(reinterpret_cast<uint8_t const*>(rawJSON.data()), rawJSON.size());*/
                return stream->tellp();
            }

          private:
            std::basic_ostream<uint8_t>* stream;
        };

        namespace internal
        {
            template <typename Type>
            auto propertyResolveToBinary(OutputArchive outputArchive, Type const& element) -> void
            {
                if constexpr (is_std_vector<Type>::value)
                {
                }
                else
                {
                    if constexpr (std::is_integral_v<Type> && !std::is_same_v<Type, bool> ||
                                  std::is_floating_point_v<Type>)
                    {
                        uint32_t value = element;
                        outputArchive.stream->write(reinterpret_cast<uint8_t*>(&value), sizeof(uint32_t));
                    }
                }
            }
        } // namespace internal
    }; // namespace serialize

    template <typename Type, typename Archive = serialize::OutputArchive>
    class Serializable
    {
      public:
        static auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<Type>
        {
            Type object = {};
            Archive archive(stream);
            try
            {
                if (archive(object) > 0)
                {
                    return object;
                }
                else
                {
                    return std::nullopt;
                }
            }
            catch (core::Exception e)
            {
                std::cerr << e.what() << std::endl;
                return std::nullopt;
            }
        }

        static auto serialize(Type const& object, std::basic_ostream<uint8_t>& stream) -> size_t
        {
            Archive archive(stream);
            return archive(object);
        }
    };

    template <typename Type, typename Archive = serialize::OutputArchive>
    auto loadFromBytes(std::span<uint8_t const> const dataBytes) -> std::optional<Type>
    {
        std::basic_ispanstream<uint8_t> stream(
            std::span<uint8_t>(const_cast<uint8_t*>(dataBytes.data()), dataBytes.size()), std::ios::binary);
        return Serializable<Type, Archive>::deserialize(stream);
    }

    template <typename Type>
    auto loadFromFile(std::filesystem::path const& filePath) -> std::optional<Type>
    {
        std::basic_ifstream<uint8_t> stream(filePath, std::ios::binary);
        if (!stream.is_open())
        {
            return std::nullopt;
        }
        return Serializable<Type>::deserialize(stream);
    }

    template <typename Type>
    auto saveToFile(Type const& object, std::filesystem::path const& filePath) -> bool
    {
        std::basic_ofstream<uint8_t> stream(filePath, std::ios::binary);
        if (!stream.is_open())
        {
            return false;
        }
        return Serializable<Type>::serialize(object, stream) > 0;
    }

    template <typename Type, typename Archive = serialize::OutputArchive>
    auto saveToBytes(Type const& object) -> std::optional<std::vector<uint8_t>>
    {
        std::basic_stringstream<uint8_t> stream;
        if (Serializable<Type, Archive>::serialize(object, stream) > 0)
        {
            return std::vector<uint8_t>(std::istreambuf_iterator<uint8_t>(stream.rdbuf()), {});
        }
        else
        {
            return std::nullopt;
        }
    }
} // namespace ionengine::core