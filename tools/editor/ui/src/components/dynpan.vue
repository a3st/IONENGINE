<template>
    <div class="dynpan-container">
        <slot></slot>
    </div>
</template>

<script>
export default {
    mounted() {
        this.onPanelCreate(this.$el);
    },
    methods: {
        onPanelCreate(e) {
            this.$parent.onPanelCreate(e);
        },
    },
};
</script>

<style>
.dynpan-container {
    display: flex;
    margin: 0;
    padding: 0;
    min-height: 150px;
    min-width: 150px;
    width: 100%;
    height: 100%;
}
</style>