<template>
    <dyngr type="row">
        <dynpan>
            <div class="pan-wrapper" style="background-color: #2f2f2f; gap: 10px;">
                <input type="text" placeholder="Type to search" />
                <div class="abrowser-folder-struct-container">
                    <dirli :item="folderItems" @click="onFolderClick($event)"></dirli>
                </div>
            </div>
        </dynpan>

        <dynpan @mousedown="onAssetBrowserMouseDown($event)">
            <div class="pan-wrapper">
                <div class="abrowser-path-container">
                    <button class="btn-text" @click="changeDirPathByIndex(0)">
                        <span>{{ basePath }}</span>
                    </button>

                    <div v-for="(path, index) in nextPaths" class="abrowser-btn-path-container">
                        <img src="images/angle-down.svg" width="12" height="12" style="transform: rotateZ(-90deg);" />

                        <button class="btn-text" @click="changeDirPathByIndex(index + 1)">
                            <span>{{ path }}</span>
                        </button>
                    </div>
                </div>

                <div class="abrowser-asset-container">
                    <asset v-for="item in curFolderItems" :name="item.name" :type="item.type"
                        @click="onAssetClick($event, item)" @dblclick="onAssetDblClick($event, item)" @rename="onAssetRename($event, item)"></asset>

                    <asset ref="temporaryAsset" v-if="temporaryAsset.show" :name="temporaryAsset.name" :type="temporaryAsset.type"
                        @rename="onAssetRename($event, null)"></asset>
                </div>
            </div>
        </dynpan>
    </dyngr>
</template>

<script>
import $ from 'jquery';
import contextMenu from '../thirdparty/context.js/context';

import DynpanComponent from '../components/dynpan.vue';
import DyngrComponent from '../components/dyngr.vue';
import DirliComponent from '../components/dirli.vue';
import AssetComponent from '../components/asset.vue';

export default {
    emits: ['open', 'create', 'delete', 'rename'],
    components: {
        'dynpan': DynpanComponent,
        'dyngr': DyngrComponent,
        'dirli': DirliComponent,
        'asset': AssetComponent
    },
    computed: {
        basePath() {
            return this.dirPaths[0];
        },
        nextPaths() {
            return this.dirPaths.slice(1, this.dirPaths.length);
        }
    },
    data() {
        return {
            dirPaths: [],
            curFolderItems: {},
            folderItems: {},
            temporaryAsset: {
                show: false,
                name: "NewFile",
                type: "file/unknown"
            }
        }
    },
    methods: {
        open(folderItems) {
            this.folderItems = folderItems;
            this.curFolderItems = folderItems.childrens;
            this.dirPaths.push(folderItems.name);
        },
        onFolderClick(e) {
            this.curFolderItems = e.target.__vueParentComponent.props.item.childrens;

            const dirPath = e.target.__vueParentComponent.props.item.path;
            const relativePath = dirPath.substring(this.folderItems.path.length + 1, dirPath.length);

            if(relativePath.length > 0) {
                this.dirPaths.splice(1, this.dirPaths.length - 1);
                this.dirPaths.push(relativePath);
            }
        },
        onAssetDblClick(e, item) {
            switch(item.type) {
                case 'folder': {
                    this.curFolderItems = item.childrens;
                    this.dirPaths.push(item.name);
                    break;
                }
                default: {
                    this.$emit('open', item);
                    break;
                }
            }
        },
        onAssetClick(e, item) {
            if(e.which == 3) {
                e.stopPropagation();

                switch(item.type) {
                    case 'folder': {
                        contextMenu.show(e, [
                            {
                                items: [
                                    {
                                        name: "Open Folder",
                                        shortcut: "Ctrl+O",
                                        action: () => {
                                            this.curFolderItems = item.childrens;
                                            this.dirPaths.push(item.name);
                                        }
                                    }
                                ]
                            },
                            {
                                items: [
                                    {
                                        name: "Rename",
                                        shortcut: "F2",
                                        action: () => {
                                            e.target.__vueParentComponent.ctx.editName();
                                        }
                                    },
                                    {
                                        name: "Delete",
                                        shortcut: "Delete",
                                        action: () => {
                                            this.$emit('delete', item);
                                        }
                                    }
                                ]
                            }
                        ]);
                        break;
                    }
                    case 'asset/shadergraph': {
                        contextMenu.show(e, [
                            {
                                items: [
                                    {
                                        name: "Open",
                                        shortcut: "Ctrl+O",
                                        action: () => {
                                            this.$emit('open', item);
                                        }
                                    }
                                ]
                            },
                            {
                                items: [
                                    {
                                        name: "Rename",
                                        shortcut: "F2",
                                        action: () => {
                                            e.target.__vueParentComponent.ctx.editName();
                                        }
                                    },
                                    {
                                        name: "Delete",
                                        shortcut: "Delete",
                                        action: () => {
                                            this.$emit('delete', item);
                                        }
                                    }
                                ]
                            }
                        ]);
                        break;
                    }
                }
            }
        },
        onAssetRename(e, item) {
            const renamedItem = {
                name: e
            };
            if(this.temporaryAsset.show) {
                this.temporaryAsset.show = false;
                renamedItem['type'] = this.temporaryAsset.type;
                renamedItem['path'] = this.curFolderItems.path;
            } else {
                item.name = e;
                renamedItem['type'] = item.type;
                renamedItem['path'] = item.path;
            }
            this.$emit('rename', e);
        },
        onAssetBrowserMouseDown(e) {
            if (e.which == 3) {
                e.stopPropagation();
                contextMenu.show(e, [
                    {
                        items: [
                            {
                                name: "New Shader Graph",
                                shortcut: null,
                                action: () => {
                                    this.temporaryAsset.show = true;
                                    this.temporaryAsset.name = 'NewFile';
                                    this.temporaryAsset.type = 'asset/shadergraph';

                                    this.$emit('create', { 
                                            'path': this.dirPaths.join('/'), 
                                            'name': this.temporaryAsset.name,
                                            'type': this.temporaryAsset.type
                                        });

                                    this.$nextTick(() => {
                                        this.$refs.temporaryAsset.editName();
                                    });
                                }
                            }
                        ]
                    }
                ]);
            }
        },
        changeDirPathByIndex(index) {
            const fs = k => {
                let stack = [], i = 0;
                stack.push(this.folderItems);
                while(stack.length > 0) {
                    const item = stack.pop();
                    if(item.name == this.dirPaths[k]) {
                        return item;
                    } else {
                        if(item.name == this.dirPaths[i]) {
                            for(let j = 0; j < item.childrens.length; j++) {
                                stack.push(item.childrens[j]);
                                i++;
                            }
                        }
                    }
                }
                return null;
            };

            this.curFolderItems = fs(index).childrens;
            if(index != this.dirPaths.length) {
                this.dirPaths = this.dirPaths.slice(0, index + 1);
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

.abrowser-folder-struct-container {
    display: flex; 
    flex-direction: column;
    gap: 5px;
}

.abrowser-btn-path-container {
    display: flex; 
    flex-direction: row; 
    align-items: center; 
    gap: 5px;
}
</style>