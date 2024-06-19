<template>
    <div v-if="this.type == 'row'" class="dyngr-container row">
        <slot></slot>
    </div>
    <div v-if="this.type == 'col'" class="dyngr-container col">
        <slot></slot>
    </div>
</template>

<script>
export default {
    props: {
        type: String
    },
    methods: {
        onPanelCreate(e) {
            let requiredParent = this.$parent;
            while(!requiredParent.onPanelCreate) {
                requiredParent = requiredParent.$parent;
            }
            requiredParent.onPanelCreate(e);
        }
    }
}
</script>

<style>
.dyngr-container {
    position: relative;
    display: flex;
    margin: 0;
    padding: 0;
    width: 100%;
    height: 100%;
}

.dyngr-container.row {
    flex-direction: row;
}

.dyngr-container.col {
    flex-direction: column;
}
</style>