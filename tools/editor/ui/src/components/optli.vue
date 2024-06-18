<template>
    <div class="optli-container">
        <span class="optli-body-container">{{ title }}</span>
        <span class="optli-body-container">
            <select v-if="type == 'select'" v-bind:value="modelValue" @change="$emit('update:modelValue', Number($event.target.value))">
                <option v-for="(type, index) in content" v-bind:value="index">{{ type }}</option>
            </select>
            <input v-else-if="type == 'checkbox'" type="checkbox" v-bind:value="modelValue" 
                @change="$emit('update:modelValue', Boolean($event.target.checked))" />
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

.optli-body-container {
    display: inline-flex; 
    width: 50%;
    align-items: center;
}

.optli-body-container.right {
    justify-content: center;
}
</style>