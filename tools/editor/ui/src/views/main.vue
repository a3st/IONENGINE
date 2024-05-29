<template>
    <toolbar></toolbar>

    <dynview ref="mainView">
        <dyngr type="row">
            <dyngr type="col">
                <dynpan>
                    <div style="display: flex; flex-direction: column; padding: 5px; width: 100%;">
                        <div style="display: flex; flex-direction: row; height: 30px; background-color: rgb(50, 50, 50); width: 100%;">
                            <button class="btn-tab" @click="onTabClick($event, 'details', 'tab-shader-graph')">
                                <div style="display: inline-flex; justify-content: center; align-items: center; gap: 10px;">
                                    <object data="images/diagram-project.svg" width="16" height="16"></object>
                                    <span>Shader Graph</span>
                                    <object data="images/xmark.svg" width="12" height="12"></object>
                                </div>
                            </button>
                        </div>
                        <div id="tab-shader-graph" class="tab-container" style="height: calc(100% - 30px);">
                            <div style="background-color: rgb(45, 45, 45); height: 30px; display: inline-flex; align-items: center; padding: 0px 10px 0px 10px;">
                                <div style="display: inline-flex; align-items: center; gap: 10px;">
                                    <button class="btn-text">Compile</button>
                                </div>
                            </div>
                            <div id="graph-container" style="height: calc(100% - 30px);">
                                <div id="graph-root"></div>
                            </div>
                        </div>
                    </div>
                </dynpan>
                <dynpan>
                </dynpan>
            </dyngr>
            <dynpan>
            </dynpan>
        </dyngr>
    </dynview>

    <!--<div style="display: flex; flex-direction: row; width: 100%; height: calc(100% - 55px);">
        <div style="display: flex; flex-direction: column; width: 61%; height: 100%;">
            <div style="background-color: rgb(30, 30, 30); height: 30px; display: inline-flex; align-items: center; padding: 0px 10px 0px 10px;">
                <div style="display: inline-flex; align-items: center; gap: 10px;">
                    <object data="images/diagram-project.svg" width="16" height="16"></object>
                    <button class="btn-text">Compile</button>
                </div>
            </div>
            <div id="graph-container" style="height: calc(100% - 30px);">
                <div id="graph-root"></div>
            </div>
        </div>
        
        <div class="panel-resizer" style="display: block; width: 8px; height: 100%;" @mousedown="onPanelResize"></div>

        <div style="display: flex; flex-direction: column; height: 100%; width: calc(40% - 8px);">
            <div id="preview" style="width: 100%; height: 30%;">
                <div style="display: flex; flex-direction: row;">
                    <button class="btn-tab" @click="onTabClick($event, 'preview', 'tab-viewport')">
                        <div style="display: inline-flex; justify-content: center; align-items: center; gap: 10px;">
                            <object data="images/video.svg" width="16" height="16"></object>
                            <span>Viewport</span>
                        </div>
                    </button>
                </div>
                <div id="tab-viewport" class="tab-container" style="height: calc(100% - 30px);"></div>
            </div>
            <div class="panel-resizer" style="display: block; width: 100%; height: 8px;" @mousedown="onPanelResize"></div>
            <div id="details" style="display: block; width: 100%; height: calc(70% - 8px);">
                <div style="display: flex; flex-direction: row;">
                    <button class="btn-tab" @click="onTabClick($event, 'details', 'tab-resources')">
                        <div style="display: inline-flex; justify-content: center; align-items: center; gap: 10px;">
                            <object data="images/images.svg" width="16" height="16"></object>
                            <span>Resources</span>
                        </div>
                    </button>

                    <button class="btn-tab" @click="onTabClick($event, 'details', 'tab-technique')">
                        <div style="display: inline-flex; justify-content: center; align-items: center; gap: 10px;">
                            <object data="images/dice-d6.svg" width="16" height="16"></object>
                            <span>Technique</span>
                        </div>
                    </button>
                </div>
                <div id="tab-resources" class="tab-container" style="height: calc(100% - 30px);">
                    <table style="display: flex; width: 100%; height: 400px;" v-show="resources.length > 0">
                        <tbody style="width: 100%; overflow-y: auto;">
                            <tr v-for="(resource, index) in resources">
                                <td style="width: 33%; text-align: center;">
                                    <input style="padding: 10px; background-color: rgb(60, 60, 60); color: white; border: none;" type="text" v-model="resource.name" />
                                </td>
                                <td style="width: 66%; text-align: center;">
                                    <select style="padding: 10px; background-color: rgb(60, 60, 60); color: white; border: none;" v-model="resource.type">
                                        <option v-for="(type, index) in resourceTypes" v-bind:value="index">{{type}}</option>
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
    </div>-->

    <footbar></footbar>
</template>

<style>
.tab-container {
    display: none;
    flex-direction: column;
    background-color: rgb(80, 80, 80);
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
import { toRaw, createApp } from 'vue'
import $ from 'jquery'
import FlowGraph from '../thirdparty/flowgraph.js/flowgraph';
import ToolbarComponent from '../components/toolbar.vue';
import FootbarComponent from '../components/footbar.vue';
import DynviewComponent from '../components/dynview.vue';
import DynpanComponent from '../components/dynpan.vue';
import DyngrComponent from '../components/dyngr.vue'

export default {
    components: {
        'toolbar': ToolbarComponent,
        'footbar': FootbarComponent,
        'dynview': DynviewComponent,
        'dynpan': DynpanComponent,
        'dyngr': DyngrComponent
    },
    data() {
        return {
            graph: null,
            resourceNodeId: -1,
            resources: [],
            resourceTypes: [
                "Texture2D",
                "float4",
                "float3",
                "float2",
                "float",
                "bool"
            ]
        };
    },
    watch: {
        resources: {
            handler(value, oldValue) {
                let posX = 0;
                let posY = 0;
                if(this.resourceNodeId != -1) {
                    [posX, posY] = toRaw(this.graph).getNode(this.resourceNodeId).position;
                    toRaw(this.graph).removeNode(this.resourceNodeId);
                    this.resourceNodeId = -1;
                }

                let outputsData = [];
                for(const resource of Object.values(value)) {
                    outputsData.push({
                        "name": resource.name,
                        "type": this.resourceTypes[resource.type]
                    });
                }
                
                if(outputsData.length > 0) {
                    this.resourceNodeId = toRaw(this.graph).addNode(posX, posY, [], outputsData, "Input Node", false, "");
                }
            },
            deep: true
        }
    },
    setup() {

    },
    created() {
        window.addEventListener("resize", this.onGraphResize);
    },
    destroyed() {
        window.removeEventListener("resize", this.onGraphResize);
    },
    mounted() {
        // const tempDiv = document.createElement('div');
        // const instance = createApp(DynpanComponent).mount(tempDiv);
        // $(this.$refs.mainView.$el).append(instance.$el);
        
        const container = $('#graph-container');
        console.log(container.width(), container.height());
        let graph = new FlowGraph(container.children(0).get(0), container.width(), container.height());

        webview.invoke('addContextItems').then(data => {
            for (const group of Object.values(data.groups)) {
                for(const node of Object.values(group.nodes)) {
                    graph.addContextItem(
                        group.name,
                        node.name,
                        (e) => {
                            graph.addNode(e.posX, e.posY, node.inputs, node.outputs, node.name, false, "");
                        }
                    );
                }
            }
        })

        graph.start();

        this.graph = graph;
    },
    methods: {
        onPanelResize() {
            console.log("resize");
        },
        onGraphResize(e) {
            const container = $('#graph-container');
            toRaw(this.graph).resize(container.width(), container.height());
        },
        onTabClick(e, groupId, tabId) {
            $(`#${groupId}`).find('.btn-tab').removeClass('active');
            
            $(e.target)
                .closest('button')
                .addClass('active');

            $(`#${groupId}`).find('.tab-container').css('display', 'none');
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
</style>