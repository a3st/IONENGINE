import 'https://code.jquery.com/jquery-3.6.0.min.js';
const $ = window.$;
import FlowGraph from '/thirdparty/flowgraph.js/flowgraph.js'

const graph = new FlowGraph($('#root').get(0));


$(document).ready(function() {
    graph.start();

    let html = `<span style="color: white;">PBR Shader</span>`;
    graph.add_node(10, 10, {}, {}, html)

    html = `<span style="color: white;">Split</span>`;
    graph.add_node(10, 10, {}, {}, html)
});