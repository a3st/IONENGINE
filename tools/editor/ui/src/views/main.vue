<template>
    <toolbar></toolbar>

    <dynview ref="mainView" @resize="onDynviewResize">
        <dyngr type="row">
            <dyngr type="col">
                <dynpan>
                    <div class="pan-wrapper">
                        <tabgr @remove="onTabRemove" @select="onTabSelect">
                            <tabli v-for="tab in tabs.workspace" :title="tab.title" :icon="tab.icon" :target="tab.target" 
                                :fixed="tab.fixed" :default="tab.default"></tabli>
                        </tabgr>

                        <tabpan v-for="tab in tabs.workspace" :id="tab.target">
                            <component :is="tab.component" v-bind="tab.props"></component>
                        </tabpan>
                    </div>
                </dynpan>
                <dynpan>
                    <div class="pan-wrapper">
                        <tabgr @remove="onTabRemove" @select="onTabSelect">
                            <tabli title="Asset Browser" icon="images/folder-tree.svg" target="tab-asset-browser" fixed default></tabli>
                        </tabgr>

                        <tabpan id="tab-asset-browser">
                            <abrowser ref="assetBrowser" 
                                @open="onAssetBrowserOpenFile" 
                                @create="onAssetBrowserCreateFile" 
                                @delete="onAssetBrowserDeleteFile"
                                @rename="onAssetBrowserRenameFile">
                            </abrowser>
                        </tabpan>
                    </div>
                </dynpan>
            </dyngr>
            <dyngr type="col">
                <dynpan>
                    <div class="pan-wrapper">
                        <tabgr @remove="onTabRemove" @select="onTabSelect">
                            <tabli v-for="tab in tabs.rightUpper" :title="tab.title" :icon="tab.icon" :target="tab.target" 
                                :fixed="tab.fixed" :default="tab.default"></tabli>
                        </tabgr>

                        <tabpan v-for="tab in tabs.rightUpper" :id="tab.target">
                            <component :is="tab.component" v-bind="tab.props"></component>
                        </tabpan>
                    </div>
                </dynpan>
                <dynpan>
                    <div class="pan-wrapper">
                        <tabgr @remove="onTabRemove" @select="onTabSelect">
                            <tabli v-for="tab in tabs.rightLower" :title="tab.title" :icon="tab.icon" :target="tab.target" 
                                :fixed="tab.fixed" :default="tab.default"></tabli>
                        </tabgr>

                        <tabpan v-for="tab in tabs.rightLower" :id="tab.target">
                            <component :is="tab.component" v-bind="tab.props"></component>
                        </tabpan>
                    </div>
                </dynpan>
            </dyngr>
        </dyngr>
    </dynview>

    <footbar></footbar>
</template>

<script>
import $ from 'jquery';

import ToolbarComponent from '../components/toolbar.vue';
import FootbarComponent from '../components/footbar.vue';
import DynviewComponent from '../components/dynview.vue';
import DynpanComponent from '../components/dynpan.vue';
import DyngrComponent from '../components/dyngr.vue';
import TabgrComponent from '../components/tabgr.vue';
import TabpanComponent from '../components/tabpan.vue';
import TabliComponent from '../components/tabli.vue';
import ABrowserComponent from '../components/abrowser.vue';
import SGEditorComponent from '../components/sgeditor.vue';
import ViewportComponent from '../components/viewport.vue';
import SGDetailsComponent from '../components/sgdetails.vue';

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
        'abrowser': ABrowserComponent,
        'sgeditor': SGEditorComponent,
        'viewport': ViewportComponent,
        'sgdetails': SGDetailsComponent
    },
    data() {
        return {
            tabs: {
                workspace: [],
                rightUpper: [],
                rightLower: []
            },
            numTabs: 0,
            editorModule: 'worldscene',
            tabFileLinks: {},
            curTabSelect: {
                workspace: null,
                rightUpper: null,
                rightLower: null
            }
        };
    },
    watch: {
        editorModule: {
            handler(value, oldValue) {
                switch(value) {
                    case 'shadergraph': {
                        this.addTabToView({
                            title: 'Viewport (Preview)', 
                            icon: 'images/video.svg',
                            fixed: true,
                            default: true,
                            component: 'viewport'
                        }, "rightUpper", true);


                        this.addTabToView({
                            title: 'Resources', 
                            icon: 'images/dice-d6.svg',
                            fixed: true,
                            default: true,
                            component: 'sgdetails',
                            props: {
                                type: 'resources'
                            }
                        }, "rightLower", true);
                    
                        this.addTabToView({
                            id: `${this.numRightLowerTabs}`,
                            title: 'Shader', 
                            icon: 'images/images.svg',
                            fixed: true,
                            default: false,
                            component: 'sgdetails',
                            props: {
                                type: 'shader'
                            }
                        }, "rightLower", false);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }
    },
    created() {
        //window.requestAnimationFrame(this.onFrameUpdate);
    },
    destroyed() {

    },
    mounted() {
        webview.invoke('getAssetTree').then(data => {
            this.$refs.assetBrowser.open(data);
        });
    },
    methods: {
        onDynviewResize(e) {
            for(const group of Object.values(this.tabs)) {
                for(const tab of Object.values(group)) {
                    $(`#${tab.target}`).children().get(0)
                        .__vueParentComponent.ctx.$forceUpdate();
                }
            }
        },
        addTabToView(tabOptions, group, activeAfter) {
            const index = this.tabs[group].push(tabOptions) - 1;
            this.tabs[group][index]['target'] = `tab-${this.numTabs}`;
            this.numTabs++;

            let externalResolve;
            const result = new Promise((resolve, _) => {
                externalResolve = resolve;
            });
                        
            this.$nextTick(() => {
                if(activeAfter) {
                    $(`#${this.tabs[group][index].target}`).parent().find('.tabgr-container').get(0)
                        .__vueParentComponent.ctx.setActiveTabByIndex(index);
                }

                $(`#${this.tabs[group][index].target}`).children().get(0)
                    .__vueParentComponent.ctx.$forceUpdate();

                externalResolve($(`#${this.tabs[group][index].target}`).get(0));
            });
            return result;
        },
        /*
            Asset Browser Callbacks
        */
        onAssetBrowserOpenFile(e) {
            webview.invoke('assetBrowserOpenFile', e).then(
                data => {
                    if(!data.error) {
                        switch(e.type) {
                            case 'asset/shadergraph': {
                                const index = this.tabs['workspace'].findIndex(x => x.path == e.path);
                                if(index != -1) {
                                    $(`#${this.tabs['workspace'][index].target}`).parent().find('.tabgr-container').get(0)
                                        .__vueParentComponent.ctx.setActiveTabByIndex(index);
                                } else {
                                    this.addTabToView({
                                        title: e.name, 
                                        icon: 'images/diagram-project.svg',
                                        fixed: false,
                                        default: false,
                                        component: 'sgeditor',
                                        path: e.path
                                    }, "workspace", true).then(element => {
                                        this.tabFileLinks[e.path] = element;
                                        element.children[0].__vueParentComponent.ctx.open(data.sceneData);
                                    })
                                }

                                this.editorModule = 'shadergraph';
                                break;
                            }
                        }
                    }
                });
        },
        onAssetBrowserDeleteFile(e) {
            webview.invoke('assetBrowserDeleteFile', e).then(
                data => {
                    if(!data.error) {
                        this.$refs.assetBrowser.deleteFile(data);
                        this.closeTabByPath(e.path);
                    }
                });
        },
        onAssetBrowserCreateFile(e) {
            webview.invoke('assetBrowserCreateFile', e).then(
                data => {
                    if(!data.error) {
                        this.$refs.assetBrowser.createFile(data);
                    }
                });
        },
        onAssetBrowserRenameFile(e) {
            webview.invoke('assetBrowserRenameFile', e).then(
                data => {
                    if(!data.error) {
                        this.$refs.assetBrowser.renameFile({
                                name: data.newName,
                                type: data.type,
                                path: data.newPath
                            },
                            {
                                name: data.oldName,
                                type: data.type,
                                path: data.oldPath
                            });
                    }
                });
        },
        onTabRemove(e) {
            e.stopPropagation();

            const findElementByTarget = target => {
                for(const [groupName, groupData] of Object.entries(this.tabs)) {
                    let i = 0;
                    for(const tab of Object.values(groupData)) {
                        if(tab.target == target) {
                            return {
                                group: groupName,
                                index: i
                            };
                        }
                        i++;
                    }
                }
                return null;
            };

            const result = findElementByTarget(e.target.__vueParentComponent.proxy.$props.target);

            if('path' in this.tabs[result.group][result.index]) {
                if(this.tabs[result.group][result.index].path in this.tabFileLinks) {
                    delete this.tabFileLinks[this.tabs[result.group][result.index].path];
                }
            }

            let changeTab = false;
            if(this.curTabSelect[result.group] == this.tabs[result.group][result.index]) {
                changeTab = true;
            }

            this.tabs[result.group].splice(result.index, 1);

            if(this.tabs[result.group].length > 0 && changeTab) {
                $(`#${e.target.__vueParentComponent.proxy.$props.target}`).parent().find('.tabgr-container').get(0)
                    .__vueParentComponent.proxy.setActiveTabByIndex(result.index == 0 ? 0 : result.index - 1);
            }
        },
        onTabSelect(e) {
            const findElementByTarget = target => {
                for(const [groupName, groupData] of Object.entries(this.tabs)) {
                    let i = 0;
                    for(const tab of Object.values(groupData)) {
                        if(tab.target == target) {
                            return {
                                group: groupName,
                                index: i
                            };
                        }
                        i++;
                    }
                }
                return null;
            };

            const result = findElementByTarget(e.__vueParentComponent.proxy.$props.target);
            this.curTabSelect[result.group] = this.tabs[result.group][result.index];

            $(`#${e.__vueParentComponent.proxy.$props.target}`).children().get(0)
                .__vueParentComponent.ctx.$forceUpdate();
        },
        closeTabByPath(path) {
            const findElementByPath = path => {
                for(const [groupName, groupData] of Object.entries(this.tabs)) {
                    let i = 0;
                    for(const tab of Object.values(groupData)) {
                        if(tab.path == path) {
                            return {
                                group: groupName,
                                index: i
                            };
                        }
                        i++;
                    }
                }
                return null;
            };

            if(path in this.tabFileLinks) {
                const result = findElementByPath(path);
                this.tabs[result.group].splice(result.index, 1);
                delete this.tabFileLinks[path];
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