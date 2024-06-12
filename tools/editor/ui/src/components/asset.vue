<template>
    <button class="asset-container" @click="onAssetClick($event)">
        <img src="test/test.png" width="64" height="64" />
        <span style="align-self: center;">{{ name }}</span>
    </button>
</template>

<script>
export default {
    emits: ['open'],
    props: {
        name: String,
        type: String
    },
    methods: {
        onAssetClick(e) {
            this.$emit('open', e);
        }
    }
}
</script>

<style>
.asset-container {
    display: flex;
    flex-direction: column;
}
</style>