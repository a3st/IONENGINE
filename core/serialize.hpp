// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/base64.hpp"
#include <simdjson.h>

namespace ionengine::core
{
    /*!
        \brief Errors that will get when deserialize or serialize
    */
    enum class serialize_error
    {
        invalid_argument,
        out_of_range,
        eof
    };

    /*!
        \brief Deserialize object with class
        \param[in] target Object that will deserialized
        \return Deserialized object or error
    */
    template <typename Archive, typename Type, typename Target>
    auto deserialize(Target const& target) -> std::expected<Type, serialize_error>
    {
        try
        {
            Type object{};
            Archive archive(const_cast<Target&>(target));
            archive(object);
            return object;
        }
        catch (std::invalid_argument e)
        {
            return std::unexpected(serialize_error::invalid_argument);
        }
        catch (std::out_of_range e)
        {
            return std::unexpected(serialize_error::out_of_range);
        }
    }

    /*!
        \brief Serialize object with class
        \param[in] object Object that will serialized
        \return Serialized object or error
    */
    template <typename Archive, typename Target, typename Type>
    auto serialize(Type const& object) -> std::expected<Target, serialize_error>
    {
        try
        {
            Target target{};
            Archive archive(target);
            if (archive(object) > 0)
            {
                return target;
            }
            else
            {
                return std::unexpected(serialize_error::eof);
            }
        }
        catch (std::invalid_argument e)
        {
            return std::unexpected(serialize_error::invalid_argument);
        }
        catch (std::out_of_range e)
        {
            return std::unexpected(serialize_error::out_of_range);
        }
    }

    /*!
        \brief Serialize object with class
        \param[in] target Target where will output serialized data
        \param[in] object Object that will serialized
        \return Serialized size or error
    */
    template <typename Archive, typename Target, typename Type>
    auto serialize(Target& target, Type const& object) -> std::expected<size_t, serialize_error>
    {
        try
        {
            Archive archive(target);
            size_t const sizeInBytes = archive(object);
            if (sizeInBytes > 0)
            {
                return sizeInBytes;
            }
            else
            {
                return std::unexpected(serialize_error::eof);
            }
        }
        catch (std::invalid_argument e)
        {
            return std::unexpected(serialize_error::invalid_argument);
        }
        catch (std::out_of_range e)
        {
            return std::unexpected(serialize_error::out_of_range);
        }
    }

    template <typename Type>
    struct serializable_enum
    {
        template <typename Archive>
        auto operator()(Archive& archive);
    };

    class serialize_ienum
    {
      public:
        serialize_ienum(std::basic_istringstream<char>& stream) : stream(&stream)
        {
        }

        template <typename Type>
        auto field(Type const& element, std::string_view const json_name) -> void
        {
            enum_fields[std::string(json_name)] = static_cast<uint32_t>(element);
        }

        template <typename Type>
        auto operator()(Type& object) -> size_t
        {
            serializable_enum<typename std::remove_const<Type>::type> target;
            target(*this);

            auto result = enum_fields.find(stream->str());
            if (result != enum_fields.end())
            {
                object = static_cast<Type>(result->second);
                return 1;
            }
            else
            {
                return 0;
            }
        }

      private:
        std::basic_istringstream<char>* stream;
        std::unordered_map<std::string, uint32_t> enum_fields;
    };

    class serialize_oenum
    {
      public:
        serialize_oenum(std::basic_ostringstream<char>& stream) : stream(&stream)
        {
        }

        template <typename Type>
        auto field(Type const& element, std::string_view const json_name) -> void
        {
            enum_fields[static_cast<uint32_t>(element)] = std::string(json_name);
        }

        template <typename Type>
        auto operator()(Type& object) -> size_t
        {
            serializable_enum<typename std::remove_const<Type>::type> target;
            target(*this);

            auto result = enum_fields.find(static_cast<uint32_t>(object));
            if (result != enum_fields.end())
            {
                stream->write(result->second.c_str(), result->second.size());
                return stream->tellp();
            }
            else
            {
                return 0;
            }
        }

      private:
        std::basic_ostringstream<char>* stream;
        std::unordered_map<uint32_t, std::string> enum_fields;
    };

    class serialize_ijson;
    class serialize_ojson;
    class serialize_iarchive;
    class serialize_oarchive;

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

        template <typename>
        struct is_std_unordered_map : std::false_type
        {
        };

        template <typename T, typename A>
        struct is_std_unordered_map<std::unordered_map<T, A>> : std::true_type
        {
        };

        template <typename>
        struct is_std_optional : std::false_type
        {
        };

        template <typename T>
        struct is_std_optional<std::optional<T>> : std::true_type
        {
        };

        template <typename Type>
        auto from_json(serialize_ijson& input, simdjson::ondemand::value it, Type& element) -> void;

        template <typename Type>
        auto to_json(serialize_ojson& output, std::string_view const json_name, Type const& element) -> void;

        template <typename Type>
        auto from_binary(serialize_iarchive& input, Type& element) -> void;

        template <typename Type>
        auto to_binary(serialize_oarchive& output, Type const& element) -> void;
    } // namespace internal

    class serialize_ijson
    {
        template <typename Type>
        friend auto internal::from_json(serialize_ijson& input, simdjson::ondemand::value it, Type& element) -> void;

      public:
        serialize_ijson(std::basic_istream<uint8_t>& stream) : stream(&stream)
        {
            json_data = std::string(std::istreambuf_iterator<uint8_t>(stream.rdbuf()), {});
            document = parser.iterate(json_data, json_data.size() + simdjson::SIMDJSON_PADDING);
        }

        template <typename Type>
        auto property(Type& element, std::string_view const json_name) -> void
        {
            internal::from_json(*this, document[json_name], element);
        }

        template <typename Type>
        auto field(Type const& element, std::string_view const json_name) -> void
        {
            enum_fields[std::string(json_name)] = static_cast<uint32_t>(element);
        }

        template <typename Type>
        auto operator()(Type& object) -> size_t
        {
            object(*this);
            return stream->tellg();
        }

      private:
        std::string json_data;
        simdjson::ondemand::parser parser;
        simdjson::ondemand::document_stream::iterator::value_type document;
        std::basic_istream<uint8_t>* stream;
        std::unordered_map<std::string, uint32_t> enum_fields;
    };

    class serialize_ojson
    {
        template <typename Type>
        friend auto internal::to_json(serialize_ojson& output, std::string_view const json_name, Type const& element)
            -> void;

      public:
        serialize_ojson(std::basic_ostream<uint8_t>& stream) : stream(&stream)
        {
            json_chunk << "{";
        }

        template <typename Type>
        auto property(Type const& element, std::string_view const json_name) -> void
        {
            internal::to_json(*this, json_name, element);
        }

        template <typename Type>
        auto field(Type const& element, std::string_view const json_name) -> void
        {
            enum_fields[static_cast<uint32_t>(element)] = std::string(json_name);
        }

        template <typename Type>
        auto operator()(Type const& object) -> size_t
        {
            const_cast<Type&>(object)(*this);

            std::string raw_json = json_chunk.str();
            raw_json = raw_json.substr(0, raw_json.size() - 1) + "}";

            stream->write(reinterpret_cast<uint8_t const*>(raw_json.data()), raw_json.size());
            return stream->tellp();
        }

      private:
        std::stringstream json_chunk;
        std::basic_ostream<uint8_t>* stream;
        std::unordered_map<uint32_t, std::string> enum_fields;
    };

    class serialize_iarchive
    {
        template <typename Type>
        friend auto internal::from_binary(serialize_iarchive& input, Type& element) -> void;

      public:
        serialize_iarchive(std::basic_istream<uint8_t>& stream) : stream(&stream)
        {
        }

        template <typename Type>
        auto property(Type& element) -> void
        {
            internal::from_binary(*this, element);
        }

        template <typename OutputArchive, typename InputArchive, typename Type>
        auto with(Type& element) -> void
        {
            size_t buffer_size;
            stream->read(reinterpret_cast<uint8_t*>(&buffer_size), sizeof(size_t));

            std::vector<uint8_t> buffer(buffer_size);
            stream->read(buffer.data(), buffer_size);

            std::basic_ispanstream<uint8_t> sstream(
                std::span<uint8_t>(const_cast<uint8_t*>(buffer.data()), buffer.size()), std::ios::binary);
            InputArchive archive(sstream);
            archive(element);
        }

        template <typename Type>
        auto operator()(Type const& object) -> size_t
        {
            const_cast<Type&>(object)(*this);
            return stream->tellg();
        }

      private:
        std::basic_istream<uint8_t>* stream;
    };

    class serialize_oarchive
    {
        template <typename Type>
        friend auto internal::to_binary(serialize_oarchive& output, Type const& element) -> void;

      public:
        serialize_oarchive(std::basic_ostream<uint8_t>& stream) : stream(&stream)
        {
        }

        template <typename Type>
        auto property(Type const& element) -> void
        {
            internal::to_binary(*this, element);
        }

        template <typename OutputArchive, typename InputArchive, typename Type>
        auto with(Type const& element) -> void
        {
            std::basic_stringstream<uint8_t> sstream;
            OutputArchive archive(sstream);
            archive(element);

            std::vector<uint8_t> buffer(std::istreambuf_iterator<uint8_t>(sstream.rdbuf()), {});
            size_t const buffer_size = buffer.size();
            stream->write(reinterpret_cast<uint8_t const*>(&buffer_size), sizeof(size_t));
            stream->write(reinterpret_cast<uint8_t const*>(buffer.data()), buffer_size);
        }

        template <typename Type>
        auto operator()(Type const& object) -> size_t
        {
            const_cast<Type&>(object)(*this);
            return stream->tellp();
        }

      private:
        std::basic_ostream<uint8_t>* stream;
    };

    namespace internal
    {
        template <typename Type>
        auto from_json(serialize_ijson& input, simdjson::ondemand::value it, Type& element) -> void
        {
            if constexpr (std::is_integral_v<Type> && !std::is_same_v<Type, bool>)
            {
                int64_t value;
                auto error = it.get_int64().get(value);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not an integral type");
                }

                element = value;
            }
            else if constexpr (std::is_integral_v<Type> && std::is_same_v<Type, bool>)
            {
                bool value;
                auto error = it.get_bool().get(value);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not a bool type");
                }

                element = value;
            }
            else if constexpr (std::is_floating_point_v<Type>)
            {
                simdjson::ondemand::number value;
                auto error = it.get_number().get(value);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not a float type");
                }

                element = static_cast<Type>(value.get_double());
            }
            else if constexpr (std::is_same_v<Type,
                                              std::basic_string<char, std::char_traits<char>, std::allocator<char>>>)
            {
                std::string_view value;
                auto error = it.get_string().get(value);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not a string type");
                }

                element = std::string(value);
            }
            else if constexpr (std::is_scoped_enum_v<Type>)
            {
                serializable_enum<Type> object;
                object(input);

                std::string_view value;
                auto error = it.get_string().get(value);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not an enum type");
                }

                auto result = input.enum_fields.find(std::string(value));
                if (result == input.enum_fields.end())
                {
                    throw std::invalid_argument("the enum type is not found");
                }

                element = static_cast<Type>(result->second);
            }
            else if constexpr (is_std_vector<Type>::value)
            {
                if constexpr (std::is_same_v<typename Type::value_type, uint8_t>)
                {
                    std::string_view value;
                    auto error = it.get_string().get(value);
                    if (error != simdjson::SUCCESS)
                    {
                        throw std::invalid_argument("the field is not a string type");
                    }

                    auto result = base64_decode(value);
                    if (!result.has_value())
                    {
                        throw std::invalid_argument("the field is not a base64 string");
                    }
                    element = std::move(result.value());
                }
                else
                {
                    simdjson::ondemand::array elements;
                    auto error = it.get_array().get(elements);
                    if (error != simdjson::SUCCESS)
                    {
                        throw std::invalid_argument("the field is not an array");
                    }

                    element.resize(elements.count_elements());

                    uint32_t i = 0;
                    for (auto e : elements)
                    {
                        from_json(input, e.value(), element[i]);
                        ++i;
                    }
                }
            }
            else if constexpr (is_std_array<Type>::value)
            {
                simdjson::ondemand::array elements;
                auto error = it.get_array().get(elements);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not an array");
                }

                auto constexpr arraySize = std::tuple_size<Type>::value;

                uint32_t i = 0;
                for (auto e : elements)
                {
                    if (i > arraySize)
                    {
                        throw std::out_of_range("the array is out of range");
                    }
                    from_json(input, e.value(), element[i]);
                    ++i;
                }
            }
            else if constexpr (is_std_unique_ptr<Type>::value)
            {
                element = std::make_unique<typename Type::element_type>();
                from_json(input, it, *element);
            }
            else if constexpr (is_std_unordered_map<Type>::value)
            {
                simdjson::ondemand::object elements;
                auto error = it.get_object().get(elements);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not an object type");
                }

                for (auto e : elements)
                {
                    std::string_view key;
                    error = e.unescaped_key().get(key);
                    if (error != simdjson::SUCCESS)
                    {
                        throw std::invalid_argument("the field is not a key");
                    }

                    typename Type::mapped_type inserted_value;
                    from_json(input, e.value(), inserted_value);

                    if constexpr (std::is_integral_v<typename Type::key_type>)
                    {
                        element[static_cast<typename Type::key_type>(std::stoi(std::string(key)))] =
                            std::move(inserted_value);
                    }
                    else if constexpr (std::is_scoped_enum_v<typename Type::key_type>)
                    {
                        serializable_enum<typename Type::key_type> object;
                        object(input);

                        auto result = input.enum_fields.find(std::string(key));
                        if (result == input.enum_fields.end())
                        {
                            throw std::invalid_argument("the enum type is not found");
                        }

                        element[static_cast<Type::key_type>(result->second)] = std::move(inserted_value);
                    }
                    else
                    {
                        element[std::string(key)] = std::move(inserted_value);
                    }
                }
            }
            else if constexpr (is_std_optional<Type>::value)
            {
                if (!it.is_null())
                {
                    typename Type::value_type inserted_value;
                    from_json(input, it, inserted_value);
                    element = std::move(inserted_value);
                }
            }
            else
            {
                simdjson::ondemand::object object;
                auto error = it.get_object().get(object);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not an object type");
                }

                std::string_view value;
                error = object.raw_json().get(value);
                if (error != simdjson::SUCCESS)
                {
                    throw std::invalid_argument("the field is not a nested json");
                }

                std::basic_spanstream<uint8_t> sstream(
                    std::span<uint8_t>(reinterpret_cast<uint8_t*>(const_cast<char*>(value.data())), value.size()));
                serialize_ijson archive(sstream);
                archive(element);
            }
        }

        template <typename Type>
        auto to_json(serialize_ojson& output, std::string_view const json_name, Type const& element) -> void
        {
            if constexpr (is_std_vector<Type>::value)
            {
                if (!json_name.empty())
                {
                    output.json_chunk << "\"" << json_name << "\":";
                }

                if constexpr (std::is_same_v<typename Type::value_type, uint8_t>)
                {
                    std::string const encoded_string = base64_encode(element);
                    output.json_chunk << "\"" << encoded_string << "\"";
                }
                else
                {
                    output.json_chunk << "[";
                    bool is_first = true;
                    for (auto const& e : element)
                    {
                        if (!is_first)
                        {
                            output.json_chunk << ",";
                        }
                        to_json(output, "", e);
                        is_first = false;
                    }
                    output.json_chunk << "]";
                }

                if (!json_name.empty())
                {
                    output.json_chunk << ",";
                }
            }
            else if constexpr (is_std_array<Type>::value)
            {
                if (!json_name.empty())
                {
                    output.json_chunk << "\"" << json_name << "\":";
                }

                output.json_chunk << "[";
                bool is_first = true;
                for (auto const& e : element)
                {
                    if (!is_first)
                    {
                        output.json_chunk << ",";
                    }
                    to_json(output, "", e);
                    is_first = false;
                }
                output.json_chunk << "]";

                if (!json_name.empty())
                {
                    output.json_chunk << ",";
                }
            }
            else if constexpr (is_std_unique_ptr<Type>::value)
            {
                to_json(output, json_name, *element);
            }
            else if constexpr (is_std_unordered_map<Type>::value)
            {
                output.json_chunk << "\"" << json_name << "\":{";
                bool is_first = true;
                for (auto const& [key, value] : element)
                {
                    if (!is_first)
                    {
                        output.json_chunk << ",";
                    }

                    if constexpr (std::is_integral_v<typename Type::key_type>)
                    {
                        output.json_chunk << "\"" << std::to_string(key) << "\":";
                        to_json(output, "", value);
                    }
                    else if constexpr (std::is_scoped_enum_v<typename Type::key_type>)
                    {
                        serializable_enum<typename Type::key_type> object;
                        object(output);

                        auto result = output.enum_fields.find(static_cast<uint32_t>(key));
                        if (result == output.enum_fields.end())
                        {
                            throw std::invalid_argument("the enum type is not found");
                        }

                        output.json_chunk << "\"" << result->second << "\":";
                        to_json(output, "", value);
                    }
                    else
                    {
                        output.json_chunk << "\"" << key << "\":";
                        to_json(output, "", value);
                    }
                    is_first = false;
                }
                output.json_chunk << "},";
            }
            else if constexpr (is_std_optional<Type>::value)
            {
                output.json_chunk << "\"" << json_name << "\":";
                if (element.has_value())
                {
                    to_json(output, "", element.value());
                }
                else
                {
                    output.json_chunk << "null";
                }
                output.json_chunk << ",";
            }
            else
            {
                if (!json_name.empty())
                {
                    output.json_chunk << "\"" << json_name << "\":";
                }

                if constexpr (std::is_integral_v<Type> && !std::is_same_v<Type, bool> || std::is_floating_point_v<Type>)
                {
                    output.json_chunk << element;
                }
                else if constexpr (std::is_integral_v<Type> && std::is_same_v<Type, bool>)
                {
                    output.json_chunk << std::boolalpha << element;
                }
                else if constexpr (std::is_same_v<
                                       Type, std::basic_string<char, std::char_traits<char>, std::allocator<char>>>)
                {
                    output.json_chunk << "\"" << element << "\"";
                }
                else if constexpr (std::is_scoped_enum_v<Type>)
                {
                    serializable_enum<Type> object;
                    object(output);

                    auto result = output.enum_fields.find(static_cast<uint32_t>(element));
                    if (result == output.enum_fields.end())
                    {
                        throw std::invalid_argument("the enum type is not found");
                    }

                    output.json_chunk << "\"" << result->second << "\"";
                }
                else
                {
                    std::basic_stringstream<uint8_t> sstream;
                    serialize_ojson archive(sstream);
                    archive(element);

                    output.json_chunk << std::string(std::istreambuf_iterator<uint8_t>(sstream.rdbuf()), {});
                }

                if (!json_name.empty())
                {
                    output.json_chunk << ",";
                }
            }
        }

        template <typename Type>
        auto from_binary(serialize_iarchive& input, Type& element) -> void
        {
            if constexpr (std::is_integral_v<Type> || std::is_floating_point_v<Type>)
            {
                input.stream->read(reinterpret_cast<uint8_t*>(&element), sizeof(Type));
            }
            else if constexpr (std::is_same_v<Type,
                                              std::basic_string<char, std::char_traits<char>, std::allocator<char>>>)
            {
                std::vector<uint8_t> buffer;
                uint8_t value = 0x1;
                while (static_cast<char>(value) != '\0')
                {
                    if (input.stream->eof())
                    {
                        throw std::out_of_range("the buffer is out of range");
                    }
                    input.stream->read(&value, 1);
                    if (static_cast<char>(value) != '\0')
                    {
                        buffer.emplace_back(value);
                    }
                }

                element = std::string(reinterpret_cast<char*>(buffer.data()), buffer.size());
            }
            else if constexpr (is_std_vector<Type>::value)
            {
                size_t num_elements = 0;
                input.stream->read(reinterpret_cast<uint8_t*>(&num_elements), sizeof(size_t));
                element.resize(num_elements);
                for (size_t const i : std::views::iota(0u, num_elements))
                {
                    from_binary(input, element[i]);
                }
            }
            else if constexpr (is_std_array<Type>::value)
            {
                for (size_t const i : std::views::iota(0u, element.size()))
                {
                    from_binary(input, element[i]);
                }
            }
            else if constexpr (std::is_scoped_enum_v<Type>)
            {
                input.stream->read(reinterpret_cast<uint8_t*>(&element), sizeof(typename std::underlying_type_t<Type>));
            }
            else
            {
                serialize_iarchive archive(*input.stream);
                archive(element);
            }
        }

        template <typename Type>
        auto to_binary(serialize_oarchive& output, Type const& element) -> void
        {
            if constexpr (is_std_vector<Type>::value)
            {
                size_t const num_elements = element.size();
                output.stream->write(reinterpret_cast<uint8_t const*>(&num_elements), sizeof(size_t));
                for (auto const& e : element)
                {
                    to_binary(output, e);
                }
            }
            else if constexpr (is_std_array<Type>::value)
            {
                for (auto const& e : element)
                {
                    to_binary(output, e);
                }
            }
            else
            {
                if constexpr (std::is_integral_v<Type> || std::is_floating_point_v<Type>)
                {
                    output.stream->write(reinterpret_cast<uint8_t const*>(&element), sizeof(Type));
                }
                else if constexpr (std::is_same_v<
                                       Type, std::basic_string<char, std::char_traits<char>, std::allocator<char>>>)
                {
                    output.stream->write(reinterpret_cast<uint8_t const*>(element.data()), element.size());
                    char end_of_string = '\0';
                    output.stream->write(reinterpret_cast<uint8_t const*>(&end_of_string), sizeof(char));
                }
                else if constexpr (std::is_scoped_enum_v<Type>)
                {
                    auto value = static_cast<typename std::underlying_type_t<Type>>(element);
                    output.stream->write(reinterpret_cast<uint8_t const*>(&value), sizeof(Type));
                }
                else
                {
                    serialize_oarchive archive(*output.stream);
                    archive(element);
                }
            }
        }
    } // namespace internal
} // namespace ionengine::core