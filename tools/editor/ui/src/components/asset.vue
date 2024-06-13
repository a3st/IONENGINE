<template>
    <button class="btn-icon">
        <div class="asset-container" @click="onAssetClick($event)">
            <img style="object-fit: contain;"
                v-bind:src="type == 'folder' ? 'images/folder.svg' : 'images/file.svg'" width="64" height="64" />
            <span style="align-self: center;" :title="name">{{ smallName }}</span>
        </div>
    </button>
</template>

<script>
export default {
    emits: ['click'],
    props: {
        name: String,
        type: String
    },
    computed: {
        smallName() {
            return this.name.length < 8 ? this.name : this.name.substring(0, 7) + "...";
        }
    },
    methods: {
        onAssetClick(e) {
            this.$emit('click', e);
        }
    }
}
</script>

<style>
.asset-container {
    display: flex;
    flex-direction: column;
    color: white;
}
</style>