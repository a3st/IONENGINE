

#include <precompiled.h>
#include <shader/technique_parser.h>

using namespace ionengine::shader;

bool TechniqueParser::parse(std::span<uint8_t const> const data) {

    
    json5::document document;

    json5::from_string(std::string_view((char*)data.data(), (char*)data.data() + data.size()), document);

    return true;
}