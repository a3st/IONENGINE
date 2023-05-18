#pragma once

namespace ie {

namespace gapi {

enum class DescriptorType {
    
};

enum class TextureDimension {
    _2D,
    Cube
};

enum class TextureFormat {
    BGRA8_UNORM,
    BGR8_UNORM,
    RGBA8_UNORM,
    R8_UNORM,
    BC1,
    BC3,
    BC4,
    BC5,
    D32_FLOAT,
    RGBA16_FLOAT
};

enum class SamplerFilter {
    Anisotropic,
    MinMagMipLinear,
    ComparisonMinMagMipLinear
};

enum class SamplerAddressMode {
    Wrap,
    Clamp,
    Mirror
};

enum class CompareOp {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

struct AdapterDesc {
    uint16_t index;
    std::string name;
    size_t memory_size;
    bool is_uma;
};

struct SamplerDesc {
    SamplerFilter filter;
    SamplerAddressMode mode_u;
    SamplerAddressMode mode_v;
    SamplerAddressMode mode_w;
    uint16_t anisotropic;
    CompareOp op;
};

}

}