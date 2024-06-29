<template>
    <div
        class="flowgraph-context-container"
        v-show="showing"
        :style="{
            transform: 'translate(' + position[0] + 'px,' + position[1] + 'px)',
        }"
    >
        <div class="flowgraph-context-header-container">
            <img
                :src="require('../../images/search.svg')"
                width="16"
                height="16"
            />
            <input type="text" placeholder="Type to search" search />
        </div>
        <div class="flowgraph-context-main-container">
            <div v-for="(items, group, index) in groups" :key="index">
                <label>{{ group }}</label>
                <div class="flowgraph-context-group">
                    <button
                        v-for="(item, index) in items"
                        :key="index"
                        @click="
                            item.action({
                                x:
                                    $parent.lastPosition[0] +
                                    (-$parent.startCanvasPosition[0] -
                                        $parent.canvasPosition[0]) -
                                    115,
                                y:
                                    $parent.lastPosition[1] +
                                    (-$parent.startCanvasPosition[1] -
                                        $parent.canvasPosition[1]) -
                                    100,
                            }),
                                close()
                        "
                    >
                        {{ item.name }}
                    </button>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import $ from "jquery";

export default {
    data() {
        return {
            groups: {},
            showing: false,
            position: [0, 0],
        };
    },
    methods: {
        addContextItem(group, item, callback) {
            if (!(group in this.groups)) {
                this.groups[group] = [];
            }
            this.groups[group].push({
                name: item,
                action: callback,
            });
        },
        show(e) {
            this.showing = true;
            this.position = [
                e.clientX,
                e.clientY - $(".flowgraph-context-container").height() / 2,
            ];
        },
        close() {
            this.showing = false;
        },
    },
};
</script>

<style>
.flowgraph-context-container {
    display: block;
    position: absolute;
    left: 0;
    right: 0;
    bottom: 0;
    top: 0;
    width: 230px;
    height: 200px;
    z-index: 1;
    overflow: hidden;
    transform-origin: center;
}

.flowgraph-context-header-container {
    display: flex;
    flex-direction: row;
    gap: 5px;
    background-color: rgb(245, 245, 245);
    border: 1px solid rgba(0, 0, 0, 0.395);
    padding: 5px;
    margin-bottom: 5px;
}

.flowgraph-context-header-container input {
    outline: none;
    width: 100%;
    border: none;
    background-color: rgb(245, 245, 245);
    color: black;
}

.flowgraph-context-main-container {
    display: flex;
    flex-direction: column;
    padding: 5px;
    max-height: calc(100% - 25px);
    overflow-y: auto;
    background-color: rgb(245, 245, 245);
    border: 1px solid rgba(0, 0, 0, 0.395);
    color: black;
}

.flowgraph-context-group {
    display: flex;
    flex-direction: column;
    border-top: 1px solid rgb(180, 180, 180);
}

.flowgraph-context-group button:first-child {
    margin-top: 5px;
}

.flowgraph-context-group button:last-child {
    margin-bottom: 5px;
}

.flowgraph-context-group button {
    display: flex;
    align-items: center;
    justify-content: start;
    outline: none;
    border: none;
    padding: 5px;
    background-color: rgb(245, 245, 245);
    font-size: 14px;
    color: black;
}

.flowgraph-context-group button:hover {
    background-color: rgb(210, 210, 210);
}
</style>