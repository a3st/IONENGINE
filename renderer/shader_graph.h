// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>
#include <renderer/shader_template.h>

#include <lib/math/vector.h>

namespace ionengine::renderer {

using ShaderInputId = uint32_t;
using ShaderId = uint32_t;
using ShaderPassId = uint32_t;

template<class Type>
struct ShaderInput {
    std::u8string name;
};

template<>
struct ShaderInput<float> {
    std::u8string name;
    float value;
    std::pair<float, float> min_max;
};

template<>
struct ShaderInput<Vector3f> {
    std::u8string name;
    Vector3f value;
};

template<>
struct ShaderInput<Vector4f> {
    std::u8string name;
    Vector4f value;
};

template<>
struct ShaderInput<Vector2f> {
    std::u8string name;
    Vector2f value;
};

template<>
struct ShaderInput<Sampler> {
    std::u8string name;
};

using ShaderInputDesc = std::variant<ShaderInput<float>, ShaderInput<Vector3f>, ShaderInput<Vector4f>, ShaderInput<Vector2f>, ShaderInput<Sampler>>;

struct ShaderPassDesc {

    using ConstBufferInfo = std::pair<uint32_t, uint32_t>;

    std::unordered_map<ShaderInputId, ConstBufferInfo> _const_buffers;
    std::unordered_map<ShaderInputId, uint32_t> _samplers;

    ShaderPassDesc& const_buffer(ShaderInputId const id, uint32_t const index, uint32_t const offset) {

        _const_buffers[id] = { index, offset };
        return *this;
    }

    ShaderPassDesc& sampler(ShaderInputId const id, uint32_t const index) {

        _samplers[id] = index;
        return *this;
    }
};

struct ShaderResultDesc {
    ShaderDomain domain;
    ShaderBlendMode blend_mode;
    ShaderPassId shader_low;
    ShaderPassId shader_high;
    CullMode cull_mode;
    FillMode fill_mode;
};

class ShaderGraph {
public:

    ShaderGraph() = default;

    ShaderGraph& input(ShaderInputId const id, ShaderInputDesc const& input_desc);

    ShaderGraph& shader(ShaderId const id, std::span<char8_t const> const data, ShaderFlags const flags);

    ShaderGraph& shader_pass(ShaderPassId const id, uint32_t pass_index, ShaderPassDesc const& desc);

    void build(Backend& backend, ShaderResultDesc const& result, ShaderTemplate& output);
    void reset();

private:

    struct Shader {
        uint32_t pass_index;
        ShaderPassDesc desc;
        std::vector<Handle<renderer::Shader>> shaders;
    };

    //using Node = std::variant<ShaderInputDesc, Shader>;
    //using NodeId = uint32_t;

    //std::unordered_map<NodeId, Node> _nodes;

    //std::unordered_map<NodeInputId, NodeId> _node_inputs;
    //std::unordered_map<NodeOutputId, NodeId> _node_outputs;

    uint32_t _node_index{0};
};

}