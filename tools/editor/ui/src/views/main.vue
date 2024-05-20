<template>
    <toolbar></toolbar>

    <div style="display: flex; flex-direction: row; width: 100%; height: calc(100% - 55px);">
        <div id="graph-container" style="width: 61%; height: 100%;">
            <div id="graph-root"></div>
        </div>

        <div class="panel-resizer" style="display: block; width: 8px; height: 100%;" @mousedown="onPanelResize"></div>

        <div style="display: flex; flex-direction: column; height: 100%; width: calc(40% - 8px);">
            <div id="preview" style="width: 100%; height: 30%;"></div>
            <div class="panel-resizer" style="display: block; width: 100%; height: 8px;" @mousedown="onPanelResize"></div>
            <div style="display: block; width: 100%; height: calc(70% - 8px);">
                <div style="display: flex; flex-direction: row;">
                    <button class="btn-tab" @click="onTabClick($event, 'tab-resources')">
                        <div style="display: inline-flex; justify-content: center; align-items: center; gap: 10px;">
                            <object data="images/images.svg" width="16" height="16"></object>
                            <span>Resources</span>
                        </div>
                    </button>

                    <button class="btn-tab" @click="onTabClick($event, 'tab-technique')">
                        <div style="display: inline-flex; justify-content: center; align-items: center; gap: 10px;">
                            <object data="images/dice-d6.svg" width="16" height="16"></object>
                            <span>Technique</span>
                        </div>
                    </button>
                </div>
                <div id="tab-resources" class="tab-container" style="height: calc(100% - 30px);">
                    <table style="display: flex; width: 100%; height: 400px;" v-show="resources.length > 0">
                        <tbody style="width: 100%; overflow-y: auto;">
                            <tr v-for="(resource, index) in resources" :key="resource.name">
                                <td style="width: 33%; text-align: center;">
                                    <input style="padding: 10px; background-color: rgb(60, 60, 60); color: white; border: none;" type="text" v-bind:value="resource.name" />
                                </td>
                                <td style="width: 66%; text-align: center;">
                                    <select style="padding: 10px; background-color: rgb(60, 60, 60); color: white; border: none;">
                                        <option>Texture2D</option>
                                        <option>float</option>
                                        <option>float2</option>
                                        <option>float3</option>
                                        <option>int</option>
                                    </select>
                                </td>
                                <td style="width: 80%; text-align: center;">
                                    <button class="btn-icon" @click="onDeleteResourceClick($event, index)">
                                        <object data="images/trash-can.svg" width="16" height="16" style="pointer-events: none;"></object>
                                    </button>
                                </td>
                            </tr>
                        </tbody>
                    </table>

                    <button class="btn" @click="onAddResourceClick">
                        <div style="display: inline-flex; justify-content: center; align-items: center; gap: 10px;">
                            <object data="images/square-plus.svg" width="16" height="16" style="pointer-events: none;"></object>
                            <span>Add Resource</span>
                        </div>
                    </button>
                </div>

                <div id="tab-technique" class="tab-container" style="height: calc(100% - 30px);">
                
                </div>
            </div>
        </div>
    </div>

    <footbar></footbar>
</template>

<style>
.tab-container {
    display: none;
    flex-direction: column; 
    padding: 15px; 
    background-color: rgb(80, 80, 80); 
    align-items: center; 
    gap: 15px;
}

.btn-icon {
    background: none;
}

.btn-icon:hover {
    background: none;
}

.btn {
    border-radius: 0px;
    font-size: 14px;
    padding: 10px 30px 10px 30px;
    background-color: rgb(45, 45, 45);
    color: white;
}

.btn:hover {
    background-color: rgb(60, 60, 60);
}

.btn-tab {
    border-radius: 0px;
    font-size: 14px;
    padding: 5px 10px 5px 10px;
    background: none;
    color: white;
}

.btn-tab.active {
    background-color: rgb(80, 80, 80);
}

.btn-tab:hover {
    background-color: rgb(80, 80, 80);
}
</style>

<script>
import { toRaw } from 'vue'
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
            graph: null,
            resourceNodeId: -1,
            resources: []
        };
    },
    watch: {
        resources: {
            handler(value, oldValue) {
                if(this.resourceNodeId != -1) {
                    toRaw(this.graph).removeNode(this.resourceNodeId);
                }

                let outputsData = [];
                for(const resource of Object.values(value)) {
                    outputsData.push({
                        "name": resource.name,
                        "type": "float"
                    });
                }

                this.resourceNodeId = toRaw(this.graph).addNode(0, 0, [], outputsData, "Input Node", false, "");
            },
            deep: true
        }
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
            toRaw(this.graph).resize(container.width(), container.height());
        },
        onTabClick(e, tabId) {
            $('.btn-tab').removeClass('active');
            
            $(e.target)
                .closest('button')
                .addClass('active');

            $('.tab-container').css('display', 'none');
            $(`#${tabId}.tab-container`).css('display', 'flex');
        },
        onAddResourceClick(e) {
            this.resources.push({
                'name': 'template',
                'type': 0
            });
        },
        onDeleteResourceClick(e, index) {
            this.resources.splice(index, 1);
        }
    }
}
</script>

<style>
@import url(../thirdparty/flowgraph.js/flowgraph.css);

.panel-resizer {
    background-color: transparent;
}

.panel-resizer:hover {
    background-color: rgb(215, 215, 215);
}
</style>