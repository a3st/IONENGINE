<template>
    <button class="btn-tab">
        <div class="tab-inline-text-container">
            <img v-bind:src="icon" width="16" height="16" />
            <span>{{ title }}</span>
            <button v-if="!fixed" class="btn-icon" @click="onRemoveClick($event)">
                <img src="images/xmark.svg" width="12" height="12" />
            </button>
        </div>
    </button>
</template>

<script>
export default {
    emits: ['remove'],
    props: {
        title: String,
        icon: String,
        target: String,
        fixed: Boolean,
        default: Boolean
    },
    methods: {
        onRemoveClick(e) {
            this.$emit('remove', e);
        }
    }
}
</script>

<style>
.tab-inline-text-container {
    display: inline-flex; 
    justify-content: center; 
    align-items: center; 
    gap: 10px;
}

.btn-tab {
    border-radius: 0px;
    font-size: 14px;
    padding: 5px 10px 5px 10px;
    background: none;
    color: white;
}

.btn-tab.active {
    background-color: rgb(80, 80, 80);
}

.btn-tab:hover {
    background-color: rgb(80, 80, 80);
}
</style>