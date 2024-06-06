<template>
    <toolbar></toolbar>

    <dynview ref="mainView" @resize="onDynviewResize">
        <dyngr type="row">
            <dyngr type="col">
                <dynpan>
                    <div class="pan-wrapper">
                        <tabgr>
                            <tabli title="Shader Graph" icon="images/diagram-project.svg" target="tab-shader-graph" fixed default></tabli>
                        </tabgr>

                        <tabpan id="tab-shader-graph">
                            <div style="background-color: rgb(45, 45, 45); height: 30px; display: inline-flex; align-items: center; padding: 0px 10px 0px 10px;">
                                <div style="display: inline-flex; align-items: center; gap: 10px;">
                                    <button class="btn-text" @click="onCompileShaderClick($event)">Compile</button>
                                </div>
                            </div>
                            <div id="graph-container" style="height: calc(100% - 30px);">
                                <div id="graph-root"></div>
                            </div>
                        </tabpan>
                    </div>
                </dynpan>
                <dynpan>
                </dynpan>
            </dyngr>
            <dyngr type="col">
                <dynpan>
                    <div class="pan-wrapper">
                        <tabgr>
                            <tabli title="Viewport (Preview)" icon="images/video.svg" target="tab-viewport" fixed default></tabli>
                        </tabgr>

                        <tabpan id="tab-viewport">
                            <img v-bind:src="'data:image/png;base64,' + previewImageSource" width="100%" height="100%" />
                        </tabpan>
                    </div>
                </dynpan>
                <dynpan>
                    <div class="pan-wrapper">
                        <tabgr>
                            <tabli title="Resources" icon="images/dice-d6.svg" target="tab-resources" fixed default></tabli>
                            <tabli title="Shader" icon="images/images.svg" target="tab-shader" fixed></tabli>
                        </tabgr>

                        <tabpan id="tab-resources">
                            <div class="pan-wrapper" style="gap: 10px;">
                                <optext v-for="(resource, index) in resources" icon="images/buffer.svg" @remove="onDeleteResourceClick($event, index)">
                                    <template #header>
                                        <input type="text" placeholder="Type name" v-model="resource.name">
                                    </template>
                                    <template #expand>
                                        <optli title="Type" type="select" v-model="resource.type" v-bind:content="resourceTypes"></optli>
                                    </template>
                                </optext>

                                <button class="btn" @click="onAddResourceClick($event)">Add New Resource</button>
                            </div>
                        </tabpan>

                        <tabpan id="tab-shader">
                            <div class="pan-wrapper" style="gap: 10px;">
                                <optgr>
                                    <optli title="Shader Domain" type="select" v-model="shaderDomain" v-bind:content="shaderDomains"></optli>
                                </optgr>
                            </div>
                        </tabpan>
                    </div>
                </dynpan>
            </dyngr>
        </dyngr>
    </dynview>

    <footbar></footbar>
</template>

<script>
import { toRaw } from 'vue'
import $ from 'jquery'
import FlowGraph from '../thirdparty/flowgraph.js/flowgraph';

import ToolbarComponent from '../components/toolbar.vue';
import FootbarComponent from '../components/footbar.vue';
import DynviewComponent from '../components/dynview.vue';
import DynpanComponent from '../components/dynpan.vue';
import DyngrComponent from '../components/dyngr.vue';
import TabgrComponent from '../components/tabgr.vue';
import TabpanComponent from '../components/tabpan.vue';
import TabliComponent from '../components/tabli.vue';
import OptextComponent from '../components/optext.vue';
import OptliComponent from '../components/optli.vue';
import OptgrComponent from '../components/optgr.vue';

export default {
    components: {
        'toolbar': ToolbarComponent,
        'footbar': FootbarComponent,
        'dynview': DynviewComponent,
        'dynpan': DynpanComponent,
        'dyngr': DyngrComponent,
        'tabgr': TabgrComponent,
        'tabli': TabliComponent,
        'tabpan': TabpanComponent,
        'optext': OptextComponent,
        'optli': OptliComponent,
        'optgr': OptgrComponent
    },
    data() {
        return {
            graph: null,
            resourceNodeId: -1,
            resources: [],
            resourceTypes: [],
            shaderDomains: [],
            domainNodes: [],
            inputNode: {},
            shaderDomain: -1,
            shaderDomainNodeId: -1,
            previewImageSource: ""
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
                    this.resourceNodeId = toRaw(this.graph).addNode(
                        posX, posY, 
                        this.inputNode.inputs, 
                        outputsData, 
                        this.inputNode.name, 
                        this.inputNode.fixed, 
                        "", 
                        this.inputNode.userData);
                }
            },
            deep: true
        },
        shaderDomain: {
            handler(value, oldValue) {
                let posX = 0;
                let posY = 0;
                if(this.shaderDomainNodeId != -1) {
                    [posX, posY] = toRaw(this.graph).getNode(this.shaderDomainNodeId).position;
                    toRaw(this.graph).removeNode(this.shaderDomainNodeId);
                    this.shaderDomainNodeId = -1;
                }
                
                const node = this.domainNodes[value];
                this.shaderDomainNodeId = toRaw(this.graph).addNode(
                    posX, posY, 
                    node.inputs, 
                    node.outputs, 
                    node.name, 
                    node.fixed, 
                    "", 
                    node.userData);
            }
        }
    },
    created() {
        window.addEventListener("resize", this.onGraphResize);
        window.requestAnimationFrame(this.onFrameUpdate);
    },
    destroyed() {
        window.removeEventListener("resize", this.onGraphResize);
    },
    mounted() {
        const graphElement = $('#graph-container');
        let graph = new FlowGraph(graphElement.children(0).get(0), graphElement.width(), graphElement.height());

        webview.invoke('addContextItems').then(data => {
            for (const node of Object.values(data.items)) {
                graph.addContextItem(
                    node.group,
                    node.name,
                    e => {
                        graph.addNode(e.posX, e.posY, node.inputs, node.outputs, node.name, false, "", node.userData);
                    }
                );
            }
        });

        webview.invoke('addResourceTypes').then(data => {
            for(const resourceType of Object.values(data.types)) {  
                this.resourceTypes.push(resourceType);
            }
        });

        webview.invoke('addShaderDomains').then(data => {
            for(const shaderDomain of Object.values(data.domains)) {
                this.shaderDomains.push(shaderDomain.name);
                this.domainNodes.push(shaderDomain.node);
            }
            this.shaderDomain = 0;
            this.inputNode = data.inputNode;
        });

        graph.start();
        this.graph = graph;
    },
    methods: {
        onDynviewResize(target) {
            const graphElement = $('#graph-container');
            toRaw(this.graph).resize(graphElement.width(), graphElement.height());
        },
        onGraphResize(e) {
            const graphElement = $('#graph-container');
            toRaw(this.graph).resize(graphElement.width(), graphElement.height());
        },
        onAddResourceClick(e) {
            this.resources.push({
                'name': 'template',
                'type': 0
            });
        },
        onDeleteResourceClick(e, index) {
            this.resources.splice(index, 1);
        },
        onFrameUpdate() {
            webview.invoke('requestPreviewImage').then(data => {
                this.previewImageSource = data;
                window.requestAnimationFrame(this.onFrameUpdate);
            });
        },
        onCompileShaderClick(e) {
            webview.invoke('compileShader', toRaw(this.graph).export()).then(data => {
                console.log(data);
            });
        }
    }
}
</script>

<style>
@import url(../thirdparty/flowgraph.js/flowgraph.css);

.pan-wrapper {
    display: flex; 
    flex-direction: column; 
    padding: 5px; 
    width: 100%;
    height: 100%;
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
</style>