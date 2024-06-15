<template>
    <div class="dirli-container">
        <div class="dirli-header-container">
            <button v-if="isFolder" class="btn-icon" @click="onFolderExpandClick($event)">
                <img v-bind:src="shown ? 'images/angle-up.svg' : 'images/angle-down.svg'" 
                    width="12" height="12" />
            </button>

            <div v-else style="width: 24px;"></div>

            <button class="btn-icon fixed" @click="onFolderOpenClick($event)">
                <img v-bind:src="shown ? 'images/folder-open.svg' : 'images/folder.svg'" 
                    width="16" height="16" />
                <span>{{ item.name }}</span>
            </button>
        </div>

        <div v-if="isFolder" class="dirli-body-container" v-show="shown">
            <dirli v-for="(child, index) in filteredFolderItems" :item="child" :key="index" :empty="child.isFolder" 
                @open="onChildFolderOpen($event)" @click="onChildFolderClick($event)"></dirli>
        </div>
    </div>
</template>

<script>
export default {
    emits: ['open', 'click'],
    props: {
        empty: Boolean,
        item: Object
    },
    data() {
        return {
            shown: false
        }
    },
    computed: {
        filteredFolderItems() {
            return this.item.childrens.filter(x => x.type == 'folder');
        },
        isFolder() {
            return this.item.type == 'folder' && this.filteredFolderItems.length > 0;
        }
    },
    methods: {
        onFolderOpenClick(e) {
            this.$emit('click', e);
            this.$emit('open', this);
        },
        onFolderExpandClick(e) {
            this.shown = !this.shown;
        },
        onChildFolderOpen(e) {
            this.$emit('open', e);
        },
        onChildFolderClick(e) {
            this.$emit('click', e)
        }
    }
}
</script>

<style>
.dirli-container {
    display: flex;
    flex-direction: column;
    gap: 5px;
}

.dirli-header-container {
    display: inline-flex;
    align-items: center;
}

.dirli-header-container > .btn-icon {
    display: inline-flex;
    align-items: center;
    gap: 5px;
    color: white;
}

.dirli-header-container:hover {
    background-color: #39678f;
}

.dirli-header-container.active {
    background-color: #335a7c;
}

.dirli-body-container {
    display: flex;
    flex-direction: column;
    margin-left: 30px;
    gap: 5px;
}
</style>