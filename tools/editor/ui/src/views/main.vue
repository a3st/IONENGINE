<template>
    <toolbar></toolbar>

    <div style="display: flex; flex-direction: row; width: 100%; height: calc(100% - 55px);">
        <div id="graph-container" style="width: 61%; height: 100%;">
            <div id="graph-root"></div>
        </div>

        <div class="panel-resizer" style="display: block; width: 8px; height: 100%;" @mousedown="onPanelResize"></div>

        <div style="display: flex; flex-direction: column; height: auto; width: calc(40% - 8px);">
            <div id="preview" style="width: 100%; height: 30%;"></div>
            <div class="panel-resizer" style="display: block; width: 100%; height: 8px;" @mousedown="onPanelResize"></div>
            <div style="width: 100%; height: calc(70% - 8px);">
                <div style="display: flex; flex-direction: column; padding: 15px;">
                    
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
            graph: null
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
        let graph = new FlowGraph(container.children(0).get(0), container.width(), container.height());

        graph.addContextItem(
            'Math',
            'Multiply',
            (e) => {
                const html = `<span style="color: white;">Multiply</span>`;
                this.graph.addNode(e.posX, e.posY, [
                    { "name": "A", "type": "float3" },
                    { "name": "B", "type": "float3" }
                ], [
                    { "name": "Result", "type": "float3" }
                ], html);
            }
        );

        graph.addContextItem(
            'Math',
            'Divide',
            (e) => {
                
            }
        );

        graph.addContextItem(
            'Math',
            'Minus',
            (e) => {
                
            }
        );
        graph.start();

        webview.invoke('newTestScene').then(data => {
            graph.importFromJSON(data);
        });

        this.graph = graph;
    },
    methods: {
        onPanelResize() {
            console.log("resize");
        },
        graphResized(e) {
            const container = $('#graph-container');
            this.graph.resize(container.width(), container.height());
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