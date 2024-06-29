<template>
    <button class="btn-tab" @click="onSelectClick($event)">
        <div class="tab-inline-text-container">
            <img v-bind:src="icon" width="16" height="16" />
            <span>{{ title }}</span>
            <button
                v-if="!fixed"
                class="btn-icon"
                @click="onRemoveClick($event)"
            >
                <img :src="require('../images/xmark.svg')" width="12" height="12" />
            </button>
        </div>
    </button>
</template>

<script>
import $ from "jquery";

export default {
    props: {
        title: String,
        icon: String,
        target: String,
        fixed: Boolean,
    },
    methods: {
        onRemoveClick(e) {
            this.$parent.$emit("remove", e);
        },
        onSelectClick(e) {
            for (const target of Object.values(this.$parent.$data.targets)) {
                $(`#${target}`).css("display", "none");
            }

            $(e.target)
                .closest(".tabgr-container")
                .find(".btn-tab")
                .removeClass("active");

            $(e.target).closest("button.btn-tab").addClass("active");

            this.$parent.$emit("select", e.target.closest("button.btn-tab"));

            $(`#${this.target}`).css("display", "flex");
        },
    },
};
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
    background-color: rgb(80, 80, 80);
    color: white;
}

.btn-tab:hover {
    background-color: rgb(70, 70, 70);
}

.btn-tab.active {
    border-top: 2px solid #335a7c;
    background-color: rgb(45, 45, 45);
}

.btn-tab.active:hover {
    background-color: rgb(45, 45, 45);
}
</style>