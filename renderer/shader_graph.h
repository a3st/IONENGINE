// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>
#include <renderer/shader_template.h>

#include <lib/math/vector.h>

namespace ionengine::renderer {

using NodeInputId = uint32_t;
using NodeOutputId = uint32_t;

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

using ShaderInputDesc = std::variant<ShaderInput<float>, ShaderInput<Vector3f>, ShaderInput<Vector4f>, ShaderInput<Vector2f>>;

struct ShaderDesc {
    std::u8string _name;
    std::span<ShaderData const> _shaders;
    std::unordered_map<NodeInputId, uint32_t> _inputs;
    
    ShaderDesc& name(std::u8string const& name) {
        
        _name = name;
        return *this;
    }

    ShaderDesc& shaders(std::span<ShaderData const> const shaders) {

        _shaders = shaders;
        return *this;
    }

    ShaderDesc& input(NodeInputId const input_id, uint32_t const index) {

        _inputs[input_id] = index;
        return *this;
    }
};

struct ShaderResultDesc {
    ShaderDomain domain;
    ShaderBlendMode blend_mode;
    NodeOutputId shader_low;
    NodeOutputId shader_high;
    CullMode cull_mode;
    FillMode fill_mode;
};

class ShaderGraph {
public:

    ShaderGraph() = default;

    ShaderGraph& input(NodeInputId const id, ShaderInputDesc const& input_desc);
    ShaderGraph& shader(NodeOutputId const id, uint32_t pass_index, ShaderDesc const& shader_desc);

    void build(Backend& backend, ShaderResultDesc const& result, ShaderTemplate& output);
    void reset();

private:

    struct Shader {
        uint32_t pass_index;
        ShaderDesc shader_desc;
        std::vector<Handle<renderer::Shader>> shaders;
    };

    using Node = std::variant<ShaderInputDesc, Shader>;
    using NodeId = uint32_t;

    std::unordered_map<NodeId, Node> _nodes;

    std::unordered_map<NodeInputId, NodeId> _node_inputs;
    std::unordered_map<NodeOutputId, NodeId> _node_outputs;

    uint32_t _node_index{0};
};

}