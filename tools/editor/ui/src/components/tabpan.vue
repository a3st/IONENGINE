<template>
    <div class="tabpan-container">
        <slot></slot>
    </div>
</template>

<style>
.tabpan-container {
    display: none;
    flex-direction: column;
    background-color: rgb(50, 50, 50);
    width: 100%;
    height: 100%;
}
</style>