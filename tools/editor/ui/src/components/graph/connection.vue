<template>
    <path
        v-if="manual"
        class="flowgraph-connection"
        :class="{ focused: focused }"
        :d="curvePreviewPath(this.startPos, this.endPos)"
    />
    <path
        v-else
        class="flowgraph-connection"
        :class="{ focused: focused }"
        :d="curvePath(this.source, this.out, this.dest, this.in)"
    />
</template>

<script>
import $ from "jquery";

export default {
    props: {
        id: Number,
        source: Number,
        out: Number,
        dest: Number,
        in: Number,
        manual: Boolean,
        startPos: Array,
        endPos: Array,
    },
    data() {
        return {
            selected: false,
            focused: false,
        };
    },
    methods: {
        focus() {
            this.focused = true;
        },
        blur() {
            this.focused = false;
        },
        curvePath(sourceNodeID, sourceIndex, destNodeID, destIndex) {
            const sourceNode =
                this.$parent.$refs.nodeInstance[
                    this.$parent.cacheNodes[sourceNodeID]
                ];

            sourceNode.connectedOutputs[sourceIndex] = true;

            const sourceElement = $(sourceNode.$el)
                .find(".flowgraph-node-io-row.right")
                .eq(sourceIndex)
                .find(".flowgraph-node-io.circle")
                .get(0);

            const destNode =
                this.$parent.$refs.nodeInstance[
                    this.$parent.cacheNodes[destNodeID]
                ];

            destNode.connectedInputs[destIndex] = true;

            const destElement = $(destNode.$el)
                .find(".flowgraph-node-io-row.left")
                .eq(destIndex)
                .find(".flowgraph-node-io.circle")
                .get(0);

            const sourceX =
                sourceNode.absolutePosition[0] +
                sourceElement.offsetLeft +
                sourceElement.offsetWidth / 2;
            const sourceY =
                sourceNode.absolutePosition[1] +
                sourceElement.offsetTop +
                sourceElement.offsetHeight / 2 +
                2;
            const destX =
                destNode.absolutePosition[0] +
                destElement.offsetLeft +
                destElement.offsetWidth / 2 -
                50;
            const destY =
                destNode.absolutePosition[1] +
                destElement.offsetTop +
                destElement.offsetHeight / 2 +
                2;
            const secondLineX = destX - sourceX;
            const secondLineY = destY - sourceY;

            return `M ${sourceX} ${sourceY} t 25 0 l ${secondLineX} ${secondLineY} t 25 0`;
        },
        curvePreviewPath(sourcePosition, destPosition) {
            const sourceX = sourcePosition[0];
            const sourceY = sourcePosition[1];
            const destX = destPosition[0];
            const destY = destPosition[1];

            const secondLineX = destX - sourceX;
            const secondLineY = destY - sourceY;

            return `M ${sourceX} ${sourceY} t 25 0 l ${secondLineX} ${secondLineY} t 25 0`;
        },
    },
};
</script>

<style>
.flowgraph-connection {
    stroke-width: 4px;
    stroke: rgb(42, 143, 42);
    fill: none;
}

.flowgraph-connection.focused {
    stroke: rgb(180, 180, 180);
    stroke-width: 5px;
}
</style>