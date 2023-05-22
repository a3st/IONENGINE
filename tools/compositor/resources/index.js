import 'https://code.jquery.com/jquery-3.6.0.min.js';
const $ = window.$;
import Drawflow from '/thirdparty/drawflow/drawflow.js'
//const Drawflow = window.Drawflow;

let editor = new Drawflow($('#drawflow')[0]);
editor.reroute = true;

editor.start();


$(document).ready(function() {
    var html = `
    <div style="width: 100%; height: 100%;">Functions</div>
    `;
    var data = { "name": '123' };

    editor.addNode('github', 2, 3, 0, 0, 'github', data, html);
});