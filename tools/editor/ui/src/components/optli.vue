<template>
    <div class="optli-container">
        <span style="display: flex; width: 50%;">{{ title }}</span>
        <span style="display: flex; width: 50%;">
            <select v-if="type == 'select'" v-bind:value="modelValue" @change="$emit('update:modelValue', Number($event.target.value))">
                <option v-for="(type, index) in content" v-bind:value="index">{{ type }}</option>
            </select>
        </span>
    </div>
</template>

<script>
export default {
    emits: ['update:modelValue'],
    props: {
        type: String,
        title: String,
        content: Array,
        modelValue: Number
    }
}
</script>

<style>
.optli-container {
    display: flex; 
    flex-direction: row; 
    width: 100%; 
    align-items: center;
}
</style>