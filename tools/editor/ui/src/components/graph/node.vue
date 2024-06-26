<template>
    <div
        class="flowgraph-node-container"
        :class="{ selected: selected, focused: focused }"
        :style="{
            width: nodeWidth + 'px',
            height: nodeHeight + 'px',
            transform:
                'translate(' + curPosition[0] + 'px,' + curPosition[1] + 'px)',
        }"
    >
        <div class="flowgraph-node-root">
            <div class="flowgraph-node-header">
                <span>{{ name }}</span>
            </div>

            <div class="flowgraph-node-main">
                <div
                    v-if="curInputs.length != 0"
                    class="flowgraph-node-io-container"
                >
                    <div
                        v-for="input in curInputs"
                        :key="input.name"
                        class="flowgraph-node-io-row left"
                    >
                        <div class="flowgraph-node-io circle"></div>
                        <span>{{ input.name }} ({{ input.type }})</span>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
export default {
    props: {
        name: String,
        inputs: Array,
        position: Array,
    },
    computed: {
        nodeWidth() {
            let maxWidth = 0;
            this.curInputs.forEach((element) => {
                maxWidth = Math.max(maxWidth, element.name.length * 30);
            });
            return maxWidth;
        },
        nodeHeight() {
            let maxHeight = 0;
            maxHeight = Math.max(maxHeight, this.curInputs.length * 30);
            return maxHeight + 40 + 10;
        },
    },
    watch: {
        inputs: {
            handler(value, oldValue) {
                this.curInputs = value;
            },
            immediate: true,
        },
        position: {
            handler(value, oldValue) {
                this.curPosition = value;
            },
            immediate: true,
        },
    },
    data() {
        return {
            curInputs: [],
            curPosition: [],
            selected: false,
            focused: false,
        };
    },
    mounted() {
        this.curPosition = [
            this.$parent.startCanvasPos[0] + this.curPosition[0],
            this.$parent.startCanvasPos[1] + this.curPosition[1],
        ];
    },
    methods: {
        select() {
            this.selected = true;
        },
        focus() {
            this.selected = false;
            this.focused = true;
        },
        setPosition(posX, posY) {
            this.curPosition = [posX, posY];
        },
        blur() {
            this.selected = false;
            this.focused = false;
        },
    },
};
</script>

<style>
.flowgraph-node-container {
    position: absolute;
    display: block;
    left: 0;
    right: 0;
    bottom: 0;
    top: 0;
    user-select: none;
    background-color: rgb(69, 78, 98);
    border: 1px solid rgba(0, 0, 0, 0.395);
}

.flowgraph-node-container.selected {
    outline: rgb(42, 143, 42) solid 2.5px;
}

.flowgraph-node-container.focused {
    outline: rgb(180, 180, 180) solid 2.5px;
}

.flowgraph-node-root {
    display: flex;
    flex-direction: column;
    width: 100%;
    height: 100%;
}

.flowgraph-node-header {
    height: 40px;
    background-color: rgb(38, 43, 53);
    color: white;
    display: flex;
    padding: 10px !important;
}

.flowgraph-node-main {
    display: flex;
    width: 100%;
    flex-direction: row;
    padding: 10px 5px 10px 5px !important;
}

.flowgraph-node-io-container {
    display: flex;
    flex-direction: column;
    gap: 10px;
}

.flowgraph-node-io-row {
    display: flex;
    flex-direction: row;
    align-items: center;
    color: white;
    gap: 10px;
}

.flowgraph-node-io-row.left {
    justify-content: start;
}

.flowgraph-node-io-row.right {
    justify-content: end;
}

.flowgraph-node-io.circle {
    display: block;
    width: 12px;
    height: 12px;
    border-radius: 8px;
    background-color: #ffffff;
}
</style>