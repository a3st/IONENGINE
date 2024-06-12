<template>
    <dyngr type="row">
        <dynpan>
            <div class="pan-wrapper" style="background-color: #2f2f2f;">
                <div style="display: flex; flex-direction: column; gap: 15px;">
                    <input type="text" placeholder="Type to search" />
                    <div class="abrowser-folderstruct-container">
                        <dirli v-for="item in items" 
                            v-bind:name="item.name" v-bind:empty="getChildrenFolders(item).length == 0"
                            @open="onFolderOpenClick($event, item)">
                            <dirli v-for="child in getChildrenFolders(item)" 
                                v-bind:name="child.name" v-bind:empty="getChildrenFolders(child).length == 0" 
                                @open="onFolderOpenClick($event, child)"></dirli>
                        </dirli>
                    </div>
                </div>
            </div>
        </dynpan>

        <dynpan>
            <div class="pan-wrapper">
                <div class="abrowser-path-container">
                    <button class="btn-text">
                        <span>{{ basePath }}</span>
                    </button>

                    <div v-for="path in dirPaths" style="display: flex; flex-direction: row; align-items: center; gap: 5px;">
                        <img src="images/angle-down.svg" width="12" height="12" style="transform: rotateZ(-90deg);" />

                        <button class="btn-text">
                            <span>{{ path }}</span>
                        </button>
                    </div>
                </div>

                <div class="abrowser-asset-container">
                    <asset v-for="item in curFolderItems" 
                        v-bind:name="item.name" v-bind:type="item.type" @open="onAssetOpenClick($event, item)"></asset>
                </div>
            </div>
        </dynpan>
    </dyngr>
</template>

<script>
import DynpanComponent from '../components/dynpan.vue';
import DyngrComponent from '../components/dyngr.vue';
import DirliComponent from '../components/dirli.vue';
import AssetComponent from '../components/asset.vue';

export default {
    components: {
        'dynpan': DynpanComponent,
        'dyngr': DyngrComponent,
        'dirli': DirliComponent,
        'asset': AssetComponent
    },
    props: {
        curPath: String
    },
    computed: {
        basePath() {
            this.dirPaths = this.curPath.split('/');
            const base = this.dirPaths[0];
            this.dirPaths.splice(0, 1);
            return base;
        }
    },
    data() {
        return {
            dirPaths: [],
            curFolderItems: [],
            items: [
                {
                    "name": "models",
                    "type": "folder",
                    "childrens": [
                        {
                            "name": "vehicle",
                            "type": "asset/model"
                        },
                        {
                            "name": "cube",
                            "type": "asset/model"
                        },
                        {
                            "name": "sphere",
                            "type": "asset/model"
                        }
                    ]
                },
                {
                    "name": "shaders",
                    "type": "folder",
                    "childrens": [
                        {
                            "name": "basic",
                            "type": "editor/shader_graph"
                        },
                        {
                            "name": "postprocess",
                            "type": "editor/shader_graph"
                        },
                        {
                            "name": "FX",
                            "type": "folder",
                            "childrens": [
                                {
                                    "name": "postprocess",
                                    "type": "editor/shader_graph"
                                }
                            ]
                        }
                    ]
                }
            ]
        }
    },
    methods: {
        getChildrenFolders(item) {
            return item.childrens.filter(x => x.type == 'folder');
        },
        onFolderOpenClick(e, item) {
            this.curFolderItems = item.childrens;
            console.log(item)
        },
        onAssetOpenClick(e, item) {
            switch(item.type) {
                case 'folder': {
                    this.curFolderItems = item.childrens;
                    break;
                }
                default: {
                    console.log('123')
                    break;
                }
            }
        }
    }
}
</script>

<style>
.abrowser-path-container {
    display: flex; 
    flex-direction: row;
    width: 100%; 
    background-color: rgb(47, 47, 47); 
    padding: 5px; 
    align-items: center; 
    gap: 5px;
}

.abrowser-asset-container {
    display: flex;
    flex-wrap: wrap;
    padding: 5px;
    gap: 5px;
    overflow-y: auto;
}

.abrowser-folderstruct-container {
    display: flex; 
    flex-direction: column;
    gap: 5px;
}
</style>