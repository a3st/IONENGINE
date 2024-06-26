<template>
    <div class="flowgraph-container">
        <div
            class="flowgraph-bg-container"
            :style="{
                width: canvasSize[0] + 'px',
                height: canvasSize[1] + 'px',
            }"
        ></div>
        <ctxmenu></ctxmenu>
        <div
            class="flowgraph-canvas"
            :style="{
                width: canvasSize[0] + 'px',
                height: canvasSize[1] + 'px',
                transform:
                    'translate(' +
                    canvasPosition[0] +
                    'px,' +
                    canvasPosition[1] +
                    'px) scale(' +
                    zoomScale +
                    ')',
            }"
        >
            <svg
                class="flowgraph-vectorlayer"
                :style="{
                    width: canvasSize[0] + 'px',
                    height: canvasSize[1] + 'px',
                }"
                xmlns="http://www.w3.org/2000/svg"
            ></svg>

            <node
                v-for="node in nodes"
                :key="node.id"
                :name="node.name"
                :inputs="node.inputs"
                :position="node.position"
            ></node>
        </div>
    </div>
</template>

<script>
import $ from "jquery";
import NodeComponent from "../graph/node.vue";
import CtxMenuComponent from "../graph/ctxmenu.vue";

export default {
    components: {
        node: NodeComponent,
        ctxmenu: CtxMenuComponent
    },
    props: {},
    computed: {
        startCanvasPos() {
            return [this.canvasSize[0] / 2, this.canvasSize[1] / 2];
        },
    },
    data() {
        return {
            canvasSize: [8096, 8096],
            nodes: [],
            dragMode: null,
            lastPosition: [0, 0],
            zoomScale: 1.0,
            selectedElement: null,
            canvasPosition: [-8096 / 2, -8096 / 2],
        };
    },
    mounted() {
        this.$el.addEventListener("mousedown", (e) => {
            if (e.which == 1) {
                this.dragNodeBeginHandler(e);
            } else if (e.which == 3) {
                //this.#rightClickHandler(e);
            }
        });

        this.$el.addEventListener("mousemove", (e) => {
            this.moveNodeHandler(e);
        });

        this.$el.addEventListener("mouseup", (e) => {
            this.dragNodeEndHandler(e);
        });

        this.$el.addEventListener("mousewheel", (e) => {
            this.zoomEventHandler(e);
        });

        $(this.$el)
            .find(".flowgraph-bg-container")
            .get(0)
            .style.setProperty("--grid-size", `${this.zoomScale * 40}px`);

        this.nodes.push({
            id: 0,
            name: "Output Node",
            inputs: [
                {
                    name: "Color",
                    type: "float3",
                },
            ],
            position: [0, 0],
        });
    },
    methods: {
        dragNodeBeginHandler(e) {
            if (
                $(e.target)
                    .closest(".flowgraph-node-io")
                    .closest(".flowgraph-node-io-row.left").length > 0
            ) {
                this.dragMode = "io.input";
            } else if (
                $(e.target)
                    .closest(".flowgraph-node-io")
                    .closest(".flowgraph-node-io-row.right").length > 0
            ) {
                this.dragMode = "io.output";
            } else if (
                $(e.target).closest(".flowgraph-node-container").length > 0
            ) {
                this.dragMode = "node";
            } else if (
                $(e.target).closest(".flowgraph-connection").length > 0
            ) {
                this.dragMode = "connection";
            } else if ($(e.target).closest(".flowgraph-canvas").length > 0) {
                this.dragMode = "canvas";
            }

            if (this.focusedElement) {
                this.focusedElement.__vueParentComponent.ctx.blur();
                this.focusedElement = null;
            }

            switch (this.dragMode) {
                case "node": {
                    this.selectedElement = $(e.target)
                        .closest(".flowgraph-node-container")
                        .get(0);

                    this.selectedElement.__vueParentComponent.ctx.select();
                    break;
                }
            }
        },
        dragNodeEndHandler(e) {
            switch (this.dragMode) {
                case "node": {
                    if (this.selectedElement) {
                        this.focusedElement = this.selectedElement;
                        this.selectedElement.__vueParentComponent.ctx.focus();
                    }
                    break;
                }
            }

            this.dragMode = "none";
            this.selectedElement = null;
        },
        moveNodeHandler(e) {
            const relativeX = e.clientX - this.lastPosition[0];
            const relativeY = e.clientY - this.lastPosition[1];
            this.lastPosition = [e.clientX, e.clientY];

            switch (this.dragMode) {
                case "canvas": {
                    const posMatrix = $(this.$el)
                        .find(".flowgraph-canvas")
                        .css("transform")
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeX;
                    const posY = Number(posMatrix[5]) + relativeY;

                    this.canvasPosition = [posX, posY];
                    break;
                }
                case "node": {
                    if (this.selectedElement) {
                        const posMatrix = $(this.selectedElement)
                            .css("transform")
                            .match(/-?[\d\.]+/g);

                        const posX =
                            Number(posMatrix[4]) + relativeX / this.zoomScale;

                        const posY =
                            Number(posMatrix[5]) + relativeY / this.zoomScale;

                        this.selectedElement.__vueParentComponent.ctx.setPosition(
                            posX,
                            posY
                        );

                        /*const nodeId = Number(
                            this.selectedElement.id.match(/-?[\d]+/g)[0]
                        );
                        const connections = Object.values(
                            this.connections
                        ).filter(
                            (value) =>
                                value.source == nodeId || value.dest == nodeId
                        );

                        this.nodes[this.nodesCache[nodeId]].position = [
                            Math.ceil(posX - this.relativeCanvasX),
                            Math.ceil(posY - this.relativeCanvasY),
                        ];

                        for (const [_, value] of Object.entries(connections)) {
                            const sourceElement = $(this.rootNode)
                                .find(
                                    `#node_${
                                        this.connections[
                                            this.connectionsCache[value.id]
                                        ].source
                                    }_out_${
                                        this.connections[
                                            this.connectionsCache[value.id]
                                        ].out
                                    }`
                                )
                                .get(0);
                            const sourceMatrix = $(this.rootNode)
                                .find(
                                    `#node_${
                                        this.connections[
                                            this.connectionsCache[value.id]
                                        ].source
                                    }`
                                )
                                .css("transform")
                                .match(/-?[\d\.]+/g);

                            const sourceX =
                                Number(sourceMatrix[4]) +
                                sourceElement.offsetLeft +
                                sourceElement.offsetWidth / 2;
                            const sourceY =
                                Number(sourceMatrix[5]) +
                                sourceElement.offsetTop +
                                sourceElement.offsetHeight / 2;

                            const destElement = $(this.rootNode)
                                .find(
                                    `#node_${
                                        this.connections[
                                            this.connectionsCache[value.id]
                                        ].dest
                                    }_in_${
                                        this.connections[
                                            this.connectionsCache[value.id]
                                        ].in
                                    }`
                                )
                                .get(0);
                            const destMatrix = $(this.rootNode)
                                .find(
                                    `#node_${
                                        this.connections[
                                            this.connectionsCache[value.id]
                                        ].dest
                                    }`
                                )
                                .css("transform")
                                .match(/-?[\d\.]+/g);

                            const destX =
                                Number(destMatrix[4]) +
                                destElement.offsetLeft +
                                destElement.offsetWidth / 2;
                            const destY =
                                Number(destMatrix[5]) +
                                destElement.offsetTop +
                                destElement.offsetHeight / 2;

                            this.#updateConnectionPosition(
                                value.id,
                                sourceX,
                                sourceY,
                                destX,
                                destY
                            );*/
                    }
                    break;
                }
            }
        },
        zoomEventHandler(e) {
            if (
                $(e.target).closest(".flowgraph-context-container").length > 0
            ) {
            } else {
                e.preventDefault();

                if (e.deltaY > 0) {
                    if (this.zoomScale > 0.4) {
                        this.zoomScale -= 0.05;
                    }
                } else {
                    if (this.zoomScale < 1.3) {
                        this.zoomScale += 0.05;
                    }
                }

                $(this.$el)
                    .find(".flowgraph-bg-container")
                    .get(0)
                    .style.setProperty(
                        "--grid-size",
                        `${this.zoomScale * 40}px`
                    );
            }
        },
    },
};
</script>

<style>
.flowgraph-container {
    position: relative;
    display: block;
    width: 100%;
    height: 100%;
    overflow: hidden;
}

.flowgraph-canvas {
    position: absolute !important;
    display: block;
}

.flowgraph-vectorlayer {
    display: block;
    position: absolute;
}

.flowgraph-bg-container {
    position: absolute;
    display: block;

    --grid-size: 30px;
    --grid-strength: 1px;
    --grid-dash: 10px;
    --grid-gap: 10px;
    --grid-color: #6e6e6e;
    --paper-color: #353535;

    background-color: var(--paper-color);
    background-size: var(--grid-dash) var(--grid-dash),
        var(--grid-size) var(--grid-size);
    background-image: linear-gradient(
            to bottom,
            transparent var(--grid-gap),
            var(--paper-color) var(--grid-gap)
        ),
        linear-gradient(
            to right,
            var(--grid-color) var(--grid-strength),
            transparent var(--grid-strength)
        ),
        linear-gradient(
            to right,
            transparent var(--grid-gap),
            var(--paper-color) var(--grid-gap)
        ),
        linear-gradient(
            to bottom,
            var(--grid-color) var(--grid-strength),
            transparent var(--grid-strength)
        );
}
</style>