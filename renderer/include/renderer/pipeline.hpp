// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.hpp>

namespace ionengine::renderer {

///
/// Rasterizer fill modes
///
enum class FillMode { Wireframe, Solid };

///
/// Rasterizer cull modes
///
enum class CullMode { None, Front, Back };

///
/// Compare operations
///
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

///
/// Blends
///
enum class Blend { Zero, One, SrcAlpha, InvSrcAlpha, BlendFactor };

///
/// Blend operations
///
enum class BlendOp { Add, Subtract, RevSubtract, Min, Max };

///
/// Pipeline rasterizer description
///
struct RasterizerDesc {
    FillMode fill_mode;
    CullMode cull_mode;
};

///
/// Pipeline depth stencil description
///
struct DepthStencilDesc {
    CompareOp depth_func;
    bool depth_write;
    bool stencil_write;
};

///
/// Pipeline blend description
///
struct BlendDesc {
    bool blend_enable;
    Blend blend_src;
    Blend blend_dst;
    BlendOp blend_op;
    Blend blend_src_alpha;
    Blend blend_dst_alpha;
    BlendOp blend_op_alpha;
};

class Pipeline {
 public:

    ///
    /// Create pipeline
    ///
    static core::Expected<std::unique_ptr<Pipeline>, std::string> create(
        

    ) noexcept;
};

}  // namespace ionengine::renderer