<template>
    <div class="dynpan-container">
        <slot></slot>
    </div>
</template>

<style>
.dynpan-container {
    display: flex;
    margin: 0;
    padding: 0;
    min-height: 150px;
    min-width: 150px;
    height: 100%;
}
</style>