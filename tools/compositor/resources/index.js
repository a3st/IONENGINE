import 'https://code.jquery.com/jquery-3.6.0.min.js';
const $ = window.$;
import '/thirdparty/drawflow/drawflow.min.js'
const Drawflow = window.Drawflow;

let editor = new Drawflow($('#drawflow')[0]);
editor.reroute = true;

editor.start();


$(document).ready(function() {
    var html = `
    <div>OLEG LOH</div>
    `;
    var data = { "name": '' };

    editor.addNode('github', 0, 1, 0, 0, 'github', data, html);
});