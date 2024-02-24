<template>
    <toolbar></toolbar>

    <div style="display: flex; flex-direction: row; width: 100%; height: calc(100% - 55px);">
        <div id="graph-container" style="width: 61%; height: 100%;">
            <div id="graph-root"></div>
        </div>

        <div class="panel-resizer" style="display: block; width: 8px; height: 100%;" @mousedown="onPanelResize"></div>

        <div style="display: flex; flex-direction: column; height: auto; width: calc(40% - 8px);">
            <div style="width: 100%; height: 30%; background-color: rgb(255, 255, 255);">
            </div>
            <div class="panel-resizer" style="display: block; width: 100%; height: 8px;" @mousedown="onPanelResize"></div>
            <div style="width: 100%; height: calc(70% - 8px);">
                <div style="display: flex; flex-direction: column; padding: 15px;">
                    <span>1</span>
                    <span>2</span>
                    <span>3</span>
                    <span>4</span>
                </div>
            </div>
        </div>
    </div>

    <footbar></footbar>
</template>

<script>
import $ from 'jquery'
import FlowGraph from '../thirdparty/flowgraph.js/flowgraph';
import ToolbarComponent from '../components/toolbar.vue';
import FootbarComponent from '../components/footbar.vue';

export default {
    components: {
        'toolbar': ToolbarComponent,
        'footbar': FootbarComponent
    },
    data() {
        return {
            flowgraph: null
        };
    },
    created() {
        window.addEventListener("resize", this.graphResized);
    },
    destroyed() {
        window.removeEventListener("resize", this.graphResized);
    },
    mounted() {
        const container = $('#graph-container');
        const graph = new FlowGraph(container.children(0).get(0), container.width(), container.height());

        graph.addContextItem(
            'Math',
            'Multiply',
            (e) => {
                const html = `<span style="color: white;">Multiply</span>`;
                graph.addNode(e.positionX, e.positionY, [
                    { "name": "A", "type": "float3" },
                    { "name": "B", "type": "float3" }
                ], [
                    { "name": "Result", "type": "float3" }
                ], html);
            }
        );
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

        html = `<span style="color: white;">Split</span>`;
        const final = graph.addNode(500, 60, [
            { "name": "Source", "type": "float4" }
        ], [
            { "name": "RGB", "type": "float3" },
            { "name": "Alpha", "type": "float" }
        ], html);

        this.flowgraph = graph;
    },
    methods: {
        onPanelResize() {
            console.log("resize");
        },
        graphResized(event) {
            const container = $('#graph-container');
            this.flowgraph.resize(container.width(), container.height());
        }
    }
}
</script>

<style>
@import url(../thirdparty/flowgraph.js/flowgraph.css);

.panel-resizer {
    background-color: transparent;
    border-radius: 2px;
}

.panel-resizer:hover {
    background-color: rgb(215, 215, 215);
}
</style>