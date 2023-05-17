#pragma once

namespace ie {

namespace gapi {

enum class TextureDimension {

};

struct Adapter {
    uint16_t index;
    std::string name;
    size_t memory_size;
    bool is_uma;
};

}

}