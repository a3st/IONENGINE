<template>
    <div class="dirli-container">
        <div class="dirli-header-container">
            <button v-if="!empty" class="btn-icon" @click="onFolderExpandClick($event)">
                <img v-bind:src="shown ? 'images/angle-up.svg' : 'images/angle-down.svg'" 
                    width="12" height="12" />
            </button>

            <div v-else style="width: 24px;"></div>

            <button class="btn-icon" @click="onFolderOpenClick($event)">
                <img v-bind:src="shown ? 'images/folder-open.svg' : 'images/folder.svg'" 
                    width="16" height="16" />
                <span>{{ name }}</span>
            </button>
        </div>

        <div v-if="!empty" class="dirli-body-container" v-show="shown">
            <slot></slot>
        </div>
    </div>
</template>

<script>
export default {
    emits: ["open"],
    props: {
        name: String,
        empty: Boolean
    },
    data() {
        return {
            shown: false
        }
    },
    methods: {
        onFolderExpandClick(e) {
            this.shown = !this.shown;
        },
        onFolderOpenClick(e) {
            this.$emit('open', e);
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