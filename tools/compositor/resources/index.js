import 'https://code.jquery.com/jquery-3.6.0.min.js';
const $ = window.$;
import FlowGraph from '/thirdparty/flowgraph.js/flowgraph.js'

const graph = new FlowGraph($('#root').get(0));


$(document).ready(function() {
    graph.start();

    let html = `<span style="color: white;">PBR Shader</span>`;
    const pbr_node = graph.add_node(10, 10, [
        { "name": "Albedo", "type": "float4" },
        { "name": "Normal", "type": "float3" },
        { "name": "Roughness", "type": "float3" },
        { "name": "Metalness", "type": "float3" }
    ], [
        { "name": "Color", "type": "float4" }
    ], html)

    html = `<span style="color: white;">Split</span>`;
    const split_node = graph.add_node(500, 10, [
        { "name": "Source", "type": "float4" },
    ], [
        { "name": "RGB", "type": "float3" },
        { "name": "Alpha", "type": "float" },
    ], html);

    console.log(pbr_node);

    graph.add_connection(pbr_node, 0, split_node, 0);
});