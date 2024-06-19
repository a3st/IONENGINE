<template>
    <div class="optgr-container">
        <slot></slot>
    </div>
</template>

<style>
.optgr-container {
    border-top: 1px solid #414141;
    display: flex;
    flex-direction: column;
    width: 100%;
    padding: 10px;
    gap: 10px;
    background-color: rgb(45, 45, 45);
}
</style>