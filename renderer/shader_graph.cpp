// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_graph.h>

#include <lib/exception.h>

using namespace ionengine::renderer;

ShaderGraph& ShaderGraph::input(ShaderInputId const id, ShaderInputDesc const& input_desc) {

    /*_nodes[_node_index] = input_desc;
    _node_inputs[id] = _node_index;
    ++_node_index;*/
    return *this;
}

ShaderGraph& ShaderGraph::shader(ShaderId const id, std::span<char8_t const> const data, ShaderFlags const flags) {

    return *this;
}

ShaderGraph& ShaderGraph::shader_pass(ShaderPassId const id, uint32_t pass_index, ShaderPassDesc const& desc) {

    /*auto shader = Shader {
        pass_index,
        shader_desc
    };

    _nodes[_node_index] = shader;
    _node_outputs[id] = _node_index;
    ++_node_index;*/
    return *this;
}

void ShaderGraph::build(Backend& backend, ShaderResultDesc const& result, ShaderTemplate& output) {

    /*std::array<NodeOutputId, 2> shader_nodes;
    shader_nodes[0] = result.shader_low;
    shader_nodes[1] = result.shader_high;

    for(uint32_t i = 0; i < 2; ++i) {

        auto it = _node_outputs.find(shader_nodes[i]);
        if(it == _node_outputs.end()) {
            throw Exception(u8"ShaderGraph: ShaderResultDesc contains invalid node outputs");
        }

        auto node_visitor = make_visitor(
            [&](Shader& shader) {
                
            },
            [&](ShaderInputDesc& input) {

            }
        );

        std::visit(node_visitor, _nodes[it->second]);
    }*/
}

void ShaderGraph::reset() {

    /*_nodes.clear();
    _node_inputs.clear();
    _node_outputs.clear();*/
}
