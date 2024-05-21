<template>
    <div style="display: flex; flex-direction: row; gap: 20px; background-color: rgb(29, 29, 29); height: 30px; padding: 5px; gap: 10px;">
        <button class="btn-text">File</button>
        <button class="btn-text">Edit</button>
        <button class="btn-text">Help</button>
    </div>
</template>

<style>
.btn-text {
    color: white;
    background: none;
    padding: 5px;
    font-size: 12px;
}
.btn-text:hover {
    background-color: rgb(39, 39, 39);
}
</style>

<script>
import { toRaw } from 'vue'
import $ from 'jquery'

export default {
    components: {
    }
}
</script>