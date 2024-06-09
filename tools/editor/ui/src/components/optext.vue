<template>
    <div class="optext-container">
        <div class="optext-header-container">
            <object v-bind:data="icon" width="16" height="16"></object>
            <slot name="header"></slot>
            <div class="optext-header-right-container">
                <button class="btn-icon" @click="onExpandClick($event)">
                    <object data="images/angle-down.svg" width="16" height="16" style="pointer-events: none;"></object>
                </button>
                <button class="btn-icon" @click="onRemoveClick($event)">
                    <object data="images/trash-can.svg" width="16" height="16" style="pointer-events: none;"></object>
                </button>
            </div>
        </div>
        <div class="optext-body-container">
            <slot name="expand"></slot>
        </div>
    </div>
</template>

<script>
import $ from 'jquery'

export default {
    emits: ['remove'],
    props: {
        icon: String
    },
    methods: {
        onRemoveClick(e) {
            this.$emit('remove', e);
        },
        onExpandClick(e) {
            const bodyElement = $(this.$el).find('.optext-body-container');
            if (bodyElement.css('display') == 'none') {
                bodyElement.css('display', 'flex');

                $(this.$el)
                    .find('.optext-header-right-container object')
                    .first()
                    .replaceWith(`<object data="images/angle-up.svg" width="16" height="16" style="pointer-events: none;"></object>`);
            } else {
                bodyElement.css('display', 'none');

                $(this.$el)
                    .find('.optext-header-right-container object')
                    .first()
                    .replaceWith(`<object data="images/angle-down.svg" width="16" height="16" style="pointer-events: none;"></object>`);
            }
        }
    }
}
</script>

<style>
.optext-container {
    display: flex;
    flex-direction: column; 
    width: 100%;
}

.optext-body-container {
    border-top: 1px solid #414141;
    display: none;
    flex-direction: column; 
    width: 100%;
    padding: 10px;
    gap: 10px;
    background-color: rgb(45, 45, 45);
}

.optext-header-container {
    display: flex; 
    flex-direction: row; 
    width: 100%;
    padding: 10px;
    gap: 10px;
    align-items: center;
    background-color: rgb(45, 45, 45);
}

.optext-header-right-container {
    display: flex; 
    flex-direction: row; 
    gap: 10px; 
    align-items: center; 
    margin-left: auto;
}
</style>