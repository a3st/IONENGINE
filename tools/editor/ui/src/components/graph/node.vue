<template>
    <div
        class="flowgraph-node-container"
        v-show="inputs.length != 0 || outputs.length != 0"
        :class="{ selected: selected, focused: focused }"
        :style="{
            width: nodeWidth + 'px',
            height: nodeHeight + 'px',
            transform:
                'translate(' +
                absolutePosition[0] +
                'px,' +
                absolutePosition[1] +
                'px)',
        }"
    >
        <div class="flowgraph-node-root">
            <div class="flowgraph-node-header">
                <span>{{ name }}</span>
            </div>

            <div class="flowgraph-node-main">
                <div
                    v-if="inputs.length != 0"
                    class="flowgraph-node-io-container"
                >
                    <div
                        v-for="(input, index) in inputs"
                        :key="input.name"
                        class="flowgraph-node-io-row left"
                    >
                        <div
                            v-if="input.type == 'Color'"
                            class="flowgraph-node-io-socket"
                        >
                            <span>{{ input.name }}</span>
                            <input
                                type="color"
                                :value="stringToColor(modelValue[input.name])"
                                :data-value-name="input.name"
                                @input="
                                    $event.target.style.setProperty(
                                        '--color',
                                        $event.target.value
                                    ),
                                        $emit('update:modelValue', {
                                            ...modelValue,
                                            [input.name]: colorToString(
                                                $event.target.value
                                            ),
                                        })
                                "
                            />
                        </div>
                        <div
                            v-else-if="input.type == 'Number'"
                            class="flowgraph-node-io-socket"
                        >
                            <span>{{ input.name }}</span>
                            <input
                                type="text"
                                :value="modelValue[input.name]"
                                :placeholder="input.name"
                                @input="
                                    ($event.target.value = $event.target.value
                                        .replace(/[^0-9.]/g, '')
                                        .replace(/(\..*?)\..*/g, '$1')
                                        .replace(/^0[^.]/, '0')),
                                        $emit(
                                            'update:modelValue',
                                            $event.target.value.toString()
                                        ),
                                        $emit('update:modelValue', {
                                            ...modelValue,
                                            [input.name]:
                                                $event.target.value.toString(),
                                        })
                                "
                            />
                        </div>
                        <div v-else class="flowgraph-node-io-socket">
                            <div
                                class="flowgraph-node-io circle"
                                :data-io-index="index"
                                :class="{
                                    connected: this.connectedInputs[index],
                                }"
                            ></div>
                            <span>{{ input.name }} ({{ input.type }})</span>
                        </div>
                    </div>
                </div>

                <div
                    v-if="outputs.length != 0"
                    class="flowgraph-node-io-container"
                >
                    <div
                        v-for="(output, index) in outputs"
                        :key="output.name"
                        class="flowgraph-node-io-row right"
                    >
                        <span>{{ output.name }} ({{ output.type }})</span>
                        <div
                            class="flowgraph-node-io circle"
                            :data-io-index="index"
                            :class="{ connected: this.connectedOutputs[index] }"
                        ></div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import $ from "jquery";

export default {
    emits: ["update:modelValue"],
    props: {
        id: Number,
        name: String,
        inputs: Array,
        outputs: Array,
        position: Array,
        modelValue: Object,
    },
    computed: {
        nodeWidth() {
            let maxInputsWidth = 0;
            this.inputs.forEach((element) => {
                maxInputsWidth = Math.max(
                    maxInputsWidth,
                    element.name.length * 13 + element.type.length * 13
                );
            });

            let maxOutputsWidth = 0;
            this.outputs.forEach((element) => {
                maxOutputsWidth = Math.max(
                    maxOutputsWidth,
                    element.name.length * 13 + element.type.length * 13
                );
            });
            return maxInputsWidth + maxOutputsWidth + 55;
        },
        nodeHeight() {
            let maxHeight = 0;
            maxHeight = Math.max(maxHeight, this.inputs.length * 40);
            maxHeight = Math.max(maxHeight, this.outputs.length * 40);
            return maxHeight + 40 + 6;
        },
        absolutePosition() {
            return [
                this.$parent.startCanvasPosition[0] + this.position[0],
                this.$parent.startCanvasPosition[1] + this.position[1],
            ];
        },
    },
    watch: {
        inputs: {
            handler(value, oldValue) {
                this.connectedInputs = new Array(value.length);
            },
            immediate: true,
        },
        outputs: {
            handler(value, oldValue) {
                this.connectedOutputs = new Array(value.length);
            },
            immediate: true,
        },
    },
    data() {
        return {
            selected: false,
            focused: false,
            connectedInputs: [],
            connectedOutputs: [],
        };
    },
    mounted() {
        $(this.$el)
            .find(".flowgraph-node-io-socket")
            .each((_, element) => {
                const inputElement = $(element).find("input[type=color]");
                const valueName = inputElement.attr("data-value-name");
                if (inputElement.length > 0) {
                    inputElement.css(
                        "--color",
                        this.stringToColor(this.modelValue[valueName])
                    );
                }
            });
    },
    methods: {
        select() {
            this.selected = true;
        },
        focus() {
            this.selected = false;
            this.focused = true;
        },
        blur() {
            this.selected = false;
            this.focused = false;
        },
        stringToColor(value) {
            // https://stackoverflow.com/questions/5623838/rgb-to-hex-and-hex-to-rgb
            const rgbToHex = (r, g, b) => {
                return (
                    "#" +
                    ((1 << 24) | (r << 16) | (g << 8) | b).toString(16).slice(1)
                );
            };

            const colorValue = value
                .split(",")
                .map((x) => Math.ceil(parseFloat(x) * 255));

            return rgbToHex(...colorValue);
        },
        colorToString(value) {
            // https://stackoverflow.com/questions/5623838/rgb-to-hex-and-hex-to-rgb
            const hexToRgb = (hex) =>
                hex
                    .replace(
                        /^#?([a-f\d])([a-f\d])([a-f\d])$/i,
                        (m, r, g, b) => "#" + r + r + g + g + b + b
                    )
                    .substring(1)
                    .match(/.{2}/g)
                    .map((x) => parseInt(x, 16));

            return hexToRgb(value)
                .map((x) => Number(x / 255).toFixed(3))
                .join(",");
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
    transform-origin: center;
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
    padding: 10px;
}

.flowgraph-node-main {
    display: flex;
    width: 100%;
    flex-direction: row;
    padding: 3px;
}

.flowgraph-node-io-container {
    display: flex;
    width: 100%;
    flex-direction: column;
    gap: 3px;
}

.flowgraph-node-io-row {
    display: flex;
    flex-direction: row;
    align-items: center;
    color: white;
    gap: 10px;
    padding: 10px 5px 10px 5px;
}

.flowgraph-node-io-row.left {
    justify-content: start;
    background-color: rgb(38, 43, 53);
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

.flowgraph-node-io.connected {
    background-color: rgb(42, 143, 42);
    outline: #ffffff solid 1px;
}

.flowgraph-node-io-socket {
    display: flex;
    flex-direction: row;
    gap: 15px;
    font-size: 14px;
    align-items: center;
}

.flowgraph-node-io-socket input[type="color"] {
    padding: 0;
    --color: black;
    width: 20px;
    height: 10px;
    position: relative;
}

.flowgraph-node-io-socket input[type="color"]::before {
    content: "";
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    pointer-events: none;
    width: 150%;
    height: 150%;
    background: var(--color);
    border-radius: 2px;
    outline: #ffffff solid 1px;
}

.flowgraph-node-io-socket input[type="text"] {
    width: 80px;
    outline: #ffffff solid 1px;
    border-radius: 2px;
}
</style>