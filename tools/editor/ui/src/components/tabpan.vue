<template>
    <div class="tabpan-container" :id="id">
        <slot></slot>
    </div>
</template>

<script>
export default {
    props: {
        id: String,
    },
};
</script>

<style>
.tabpan-container {
    display: none;
    flex-direction: column;
    background-color: rgb(50, 50, 50);
    width: 100%;
    height: calc(100% - 30px);
}
</style>