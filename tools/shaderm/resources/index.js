import 'https://code.jquery.com/jquery-3.6.0.min.js';
const $ = window.$;
import FlowGraph from '/thirdparty/flowgraph.js/flowgraph.js'

const graph = new FlowGraph($('#root').get(0));


$(document).ready(function() {
    graph.start();

    let html = `<span style="color: white;">PBR Shader</span>`;
    const pbr_node = graph.addNode(10, 10, [
        { "name": "Albedo", "type": "float4" },
        { "name": "Normal", "type": "float3" },
        { "name": "Roughness", "type": "float3" },
        { "name": "Metalness", "type": "float3" }
    ], [
        { "name": "Color", "type": "float4" }
    ], html)

    html = `<span style="color: white;">Split</span>`;
    const split_node = graph.addNode(500, 60, [
        { "name": "Source", "type": "float4" }
    ], [
        { "name": "RGB", "type": "float3" },
        { "name": "Alpha", "type": "float" }
    ], html);

    graph.addConnection(pbr_node.id, 0, split_node.id, 0);

    html = `<span style="color: white;">Multiply</span>`;
    const mul_node = graph.addNode(1000, 10, [
        { "name": "A", "type": "float3" },
        { "name": "B", "type": "float3" }
    ], [
        { "name": "Result", "type": "float3" }
    ], html);

    //graph.addConnection(split_node, 0, mul_node, 0);
});