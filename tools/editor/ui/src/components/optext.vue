<template>
    <div class="optext-container">
        <div class="optext-header-container">
            <img :src="icon" width="16" height="16" />
            <slot name="header"></slot>
            <div class="optext-header-right-container">
                <button class="btn-icon" @click="onExpandClick($event)">
                    <img
                        :src="
                            shown
                                ? require('../images/angle-up.svg')
                                : require('../images/angle-down.svg')
                        "
                        width="16"
                        height="16"
                    />
                </button>
                <button class="btn-icon" @click="onRemoveClick($event)">
                    <img
                        :src="require('../images/trash-can.svg')"
                        width="16"
                        height="16"
                    />
                </button>
            </div>
        </div>
        <div class="optext-body-container" v-if="shown">
            <slot name="expand"></slot>
        </div>
    </div>
</template>

<script>
export default {
    emits: ["remove"],
    props: {
        icon: String,
    },
    data() {
        return {
            shown: false,
        };
    },
    methods: {
        onRemoveClick(e) {
            this.$emit("remove", e);
        },
        onExpandClick(e) {
            this.shown = !this.shown;
        },
    },
};
</script>

<style>
.optext-container {
    display: flex;
    flex-direction: column;
    width: 100%;
}

.optext-body-container {
    border-top: 1px solid #414141;
    flex-direction: column;
    width: 100%;
    padding: 10px;
    gap: 10px;
    background-color: rgb(55, 55, 55);
}

.optext-header-container {
    display: flex;
    flex-direction: row;
    width: 100%;
    padding: 10px;
    gap: 10px;
    align-items: center;
    background-color: rgb(55, 55, 55);
}

.optext-header-right-container {
    display: flex;
    flex-direction: row;
    gap: 10px;
    align-items: center;
    margin-left: auto;
}
</style>