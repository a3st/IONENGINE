<template>
    <toolbar></toolbar>

    <dynview ref="mainView" @resize="onDynviewResize">
        <dyngr type="row">
            <dyngr type="col">
                <dynpan ref="mainWorkspace">
                    <div class="pan-wrapper">
                        <tabgr @remove="onMainWorkspaceTabRemove">
                            <tabli v-for="tab in mainWorkspaceTabs" :title="tab.title" :icon="tab.icon" :target="tab.target"></tabli>
                        </tabgr>

                        <tabpan v-for="tab in mainWorkspaceTabs" :id="tab.target">
                            <component :is="tab.component"></component>
                        </tabpan>
                    </div>
                </dynpan>
                <dynpan>
                    <div class="pan-wrapper">
                        <tabgr>
                            <tabli title="Asset Browser" icon="images/folder-tree.svg" target="tab-asset-browser" fixed default></tabli>
                        </tabgr>

                        <tabpan id="tab-asset-browser">
                            <abrowser ref="assetBrowser" @open="onAssetBrowserOpenFile($event)"></abrowser>
                        </tabpan>
                    </div>
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
import { toRaw, render, h } from 'vue'
import $ from 'jquery'

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
import ABrowserComponent from '../components/abrowser.vue';
import SGEditorComponent from '../components/sgeditor.vue';

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
        'optgr': OptgrComponent,
        'abrowser': ABrowserComponent,
        'sgeditor': SGEditorComponent
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
            previewImageSource: "",
            assetBrowserCtxMenu: null,
            mainWorkspaceTabs: [],
            numMainTabs: 0
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
        window.requestAnimationFrame(this.onFrameUpdate);
    },
    destroyed() {

    },
    mounted() {
        /*

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

        */

        webview.invoke('getAssetTree').then(data => {
            this.$refs.assetBrowser.open(data);
        });
    },
    methods: {
        onDynviewResize(e) {
            for(const tab of Object.values(this.mainWorkspaceTabs)) {
                $(`#tab-mainWorkspace-${tab.id}`).children().get(0)
                    .__vueParentComponent.ctx.$forceUpdate();
            }
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
            console.log(JSON.stringify(toRaw(this.graph).export()))
            webview.invoke('compileShader', toRaw(this.graph).export()).then(data => {
                // TODO!
            });
        },
        onAssetBrowserOpenFile(e) {
            switch(e.type) {
                case 'asset/shadergraph': {
                    const index = this.mainWorkspaceTabs.findIndex(x => x.path == e.path);
                    if(index != -1) {
                        $(this.$refs.mainWorkspace.$el).find('.tabgr-container').get(0)
                            .__vueParentComponent.ctx.setActiveTabByIndex(index);
                    } else {
                        const size = this.mainWorkspaceTabs.push({
                            id: `${this.numMainTabs}`,
                            title: e.name, 
                            icon: 'images/diagram-project.svg',
                            target: `tab-mainWorkspace-${this.numMainTabs}`,
                            component: 'sgeditor',
                            path: e.path
                        });

                        this.$nextTick(() => {
                            $(this.$refs.mainWorkspace.$el).find('.tabgr-container').get(0)
                                .__vueParentComponent.ctx.setActiveTabByIndex(size - 1);

                            $(`#tab-mainWorkspace-${this.numMainTabs}`).children().get(0)
                                .__vueParentComponent.ctx.$forceUpdate();

                            this.numMainTabs++;
                        });
                    }
                    break;
                }
            }
        },
        onMainWorkspaceTabRemove(e) {
            const index = this.mainWorkspaceTabs.findIndex(x => x.target == e.__vueParentComponent.props.target);
            this.mainWorkspaceTabs.splice(index, 1);

            if(this.mainWorkspaceTabs.length > 0) {
                $(this.$refs.mainWorkspace.$el).find('.tabgr-container').get(0)
                    .__vueParentComponent.ctx.setActiveTabByIndex(index == 0 ? 0 : index - 1);
            }
        }
    }
}
</script>

<style>
@import url('../thirdparty/flowgraph.js/flowgraph.css');
@import url('../thirdparty/context.js/context.css');

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

.btn-icon:hover > img {
    filter: invert(25%) sepia(30%) saturate(0%) hue-rotate(273deg) brightness(92%) contrast(88%);
}

.btn-icon.fixed:hover > img {
    filter: none;
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