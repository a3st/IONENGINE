<template>
    <button class="btn-tab">
        <div class="tab-inline-text-container">
            <object v-bind:data="icon" width="16" height="16"></object>
                <span>{{ title }}</span>
                <object v-if="!fixed" data="images/xmark.svg" width="12" height="12"></object>
        </div>
    </button>
</template>

<script>
export default {
    props: {
        title: String,
        icon: String,
        target: String,
        fixed: Boolean,
        default: Boolean
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