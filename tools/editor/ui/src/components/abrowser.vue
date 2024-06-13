<template>
    <dyngr type="row">
        <dynpan>
            <div class="pan-wrapper" style="background-color: #2f2f2f; gap: 10px;">
                <input type="text" placeholder="Type to search" />
                <div class="abrowser-folder-struct-container">
                    <dirli :item="folderItems" @open="onFolderOpen($event)" @click="onFolderClick($event)"></dirli>
                </div>
            </div>
        </dynpan>

        <dynpan>
            <div class="pan-wrapper">
                <div class="abrowser-path-container">
                    <button class="btn-text" @click="changeDirPathByIndex(0)">
                        <span>{{ basePath }}</span>
                    </button>

                    <div v-for="(path, index) in nextPaths" style="display: flex; flex-direction: row; align-items: center; gap: 5px;">
                        <img src="images/angle-down.svg" width="12" height="12" style="transform: rotateZ(-90deg);" />

                        <button class="btn-text" @click="changeDirPathByIndex(index + 1)">
                            <span>{{ path }}</span>
                        </button>
                    </div>
                </div>

                <div class="abrowser-asset-container">
                    <asset v-for="item in curFolderItems" :name="item.name" :type="item.type" 
                        @click="onAssetClick($event, item)"></asset>
                </div>
            </div>
        </dynpan>
    </dyngr>
</template>

<script>
import $ from 'jquery';

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
            folderItems: {}
        }
    },
    methods: {
        open(folderItems) {
            this.folderItems = folderItems;
            this.curFolderItems = folderItems.childrens;
            this.dirPaths.push(folderItems.name);
        },
        onFolderOpen(e) {
            this.curFolderItems = e.item.childrens;
        },
        onFolderClick(e) {
            let currentTarget = $(e.target).closest('.dirli-container');
            
            this.dirPaths.splice(0, this.dirPaths.length);            
            this.dirPaths.unshift($(currentTarget).find('button').children('span').html());

            while(!currentTarget.parent().hasClass('abrowser-folder-struct-container')) {
                currentTarget = currentTarget.parent().closest('.dirli-container');
                this.dirPaths.unshift($(currentTarget).find('button').children('span').html());
            }
        },
        onAssetClick(e, item) {
            switch(item.type) {
                case 'folder': {
                    this.curFolderItems = item.childrens;
                    this.dirPaths.push(item.name);
                    break;
                }
                default: {
                    break;
                }
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
</style>