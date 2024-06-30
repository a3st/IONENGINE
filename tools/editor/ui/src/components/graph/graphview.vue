<template>
    <div class="flowgraph-container">
        <div
            class="flowgraph-bg-container"
            :style="{
                width: canvasSize[0] + 'px',
                height: canvasSize[1] + 'px',
            }"
        ></div>

        <ctxmenu ref="ctxMenu"></ctxmenu>

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
                xmlns="http://www.w3.org/2000/svg"
            >
                <connection
                    v-if="previewConnection"
                    :start-pos="previewPosition"
                    :end-pos="dummyPosition"
                    manual
                ></connection>

                <connection
                    v-for="connection in connections"
                    :key="connection.id"
                    :id="connection.id"
                    :source="connection.source"
                    :out="connection.out"
                    :dest="connection.dest"
                    :in="connection.in"
                ></connection>
            </svg>

            <div
                ref="dummyPointer"
                v-show="previewConnection"
                class="flowgraph-dummy-pointer"
                :style="{
                    transform:
                        'translate(' +
                        this.dummyPosition[0] +
                        'px,' +
                        this.dummyPosition[1] +
                        'px) scale(' +
                        this.zoomScale +
                        ')',
                }"
            ></div>

            <node
                ref="nodeInstance"
                v-for="node in nodes"
                :key="node.id"
                :id="node.id"
                :name="node.name"
                :inputs="node.inputs"
                :outputs="node.outputs"
                :position="node.position"
                v-model="node.userData.options"
            ></node>
        </div>
    </div>
</template>

<script>
import $ from "jquery";
import { toRaw } from "vue";
import NodeComponent from "../graph/node.vue";
import CtxMenuComponent from "../graph/ctxmenu.vue";
import ConnectionComponent from "../graph/connection.vue";

export default {
    components: {
        node: NodeComponent,
        ctxmenu: CtxMenuComponent,
        connection: ConnectionComponent,
    },
    props: {},
    computed: {
        startCanvasPosition() {
            return [this.canvasSize[0] / 2, this.canvasSize[1] / 2];
        },
    },
    data() {
        return {
            canvasSize: [16384, 16384],
            nodes: [],
            connections: [],
            cacheNodes: {},
            cacheConnections: {},
            dragMode: null,
            lastPosition: [0, 0],
            zoomScale: 1.0,
            selectedElement: null,
            canvasPosition: [-16384 / 2, -16384 / 2],
            lastCreatedID: 0,
            previewConnection: false,
            previewPosition: [0, 0],
            dummyPosition: [0, 0],
        };
    },
    mounted() {
        this.$el.addEventListener("mousedown", (e) => {
            if (e.which == 1) {
                this.dragNodeBeginHandler(e);
            } else if (e.which == 3) {
                this.$refs.ctxMenu.show(e);
            }
        });

        this.$el.addEventListener("contextmenu", (e) => e.preventDefault());

        this.$el.addEventListener("mousemove", (e) => {
            this.moveNodeHandler(e);
        });

        this.$el.addEventListener("mouseup", (e) => {
            this.dragNodeEndHandler(e);
        });

        this.$el.addEventListener("mousewheel", (e) => {
            this.zoomEventHandler(e);
        });

        document.addEventListener("keydown", (e) => {
            this.keyHandler(e);
        });

        $(this.$el)
            .find(".flowgraph-bg-container")
            .get(0)
            .style.setProperty("--grid-size", `${this.zoomScale * 40}px`);
    },
    methods: {
        export() {
            for (const value of Object.values(this.nodes)) {
                value.position = [
                    Math.ceil(value.position[0]),
                    Math.ceil(value.position[1]),
                ];
            }
            return {
                nodes: this.nodes,
                connections: this.connections,
            };
        },
        import(graphData) {
            for (const node of Object.values(graphData.nodes)) {
                const createdIndex = this.nodes.push({
                    id: node.id,
                    name: node.name,
                    position: node.position,
                    inputs: node.inputs,
                    outputs: node.outputs,
                    fixed: node.fixed,
                    userData: node.userData,
                });

                this.cacheNodes[node.id] = createdIndex - 1;
            }

            this.$nextTick(() => {
                for (const connection of Object.values(graphData.connections)) {
                    const createdIndex = this.connections.push({
                        id: connection.id,
                        source: connection.source,
                        out: connection.out,
                        dest: connection.dest,
                        in: connection.in,
                    });

                    this.cacheConnections[connection.id] = createdIndex - 1;
                }

                const connectionsIDs = Object.keys(this.cacheConnections).map(
                    (x) => parseInt(x)
                );
                const nodesIDs = Object.keys(this.cacheNodes).map((x) =>
                    parseInt(x)
                );
                this.lastCreatedID =
                    Math.max(...connectionsIDs, ...nodesIDs) + 1;
            });
        },
        getNodesByName(nodeName) {
            return this.nodes.filter((x) => x.name == nodeName);
        },
        getNode(nodeID) {
            if (this.cacheNodes[nodeID] == null) {
                throw new Error("Node with this ID does not exist");
            }
            return this.nodes[this.cacheNodes[nodeID]];
        },
        updateNode(
            nodeID,
            posX,
            posY,
            inputs,
            outputs,
            nodeName,
            nodeFixed,
            nodeUserData
        ) {
            if (this.cacheNodes[nodeID] == null) {
                throw new Error("Node with this ID does not exist");
            }

            const node = this.nodes[this.cacheNodes[nodeID]];
            node.posX = posX;
            node.posY = posY;
            node.inputs = inputs;
            node.outputs = outputs;
            node.nodeName = nodeName;
            node.fixed = nodeFixed;
            node.userData = nodeUserData;

            const connections = Object.values(this.connections).filter(
                (element) =>
                    element.dest == node.id || element.source == node.id
            );
            for (const connection of Object.values(connections)) {
                this.removeConnection(connection.id);
            }
        },
        addNode(
            posX,
            posY,
            inputs,
            outputs,
            nodeName,
            nodeFixed,
            nodeUserData
        ) {
            const createdID = this.lastCreatedID;
            const createdIndex = this.nodes.push({
                id: createdID,
                name: nodeName,
                position: [posX, posY],
                inputs: inputs,
                outputs: outputs,
                fixed: nodeFixed,
                userData: {
                    componentID: nodeUserData.componentID,
                    options: { ...nodeUserData.options },
                },
            });

            this.cacheNodes[createdID] = createdIndex - 1;
            this.lastCreatedID++;
        },
        removeNode(nodeID) {
            if (this.cacheNodes[nodeID] == null) {
                throw new Error("Node with this ID does not exist");
            }

            if (this.nodes[this.cacheNodes[nodeID]].fixed) {
            } else {
                for (const connection of Object.values(this.connections)) {
                    if (
                        connection.source == nodeID ||
                        connection.dest == nodeID
                    ) {
                        this.removeConnection(connection.id);
                    }
                }

                this.nodes.splice(this.cacheNodes[nodeID], 1);
                this.cacheNodes = {};
                for (let i = 0; i < this.nodes.length; i++) {
                    this.cacheNodes[this.nodes[i].id] = i;
                }
            }
        },
        addConnection(sourceNodeID, sourceIndex, destNodeID, destIndex) {
            const createdID = this.lastCreatedID;
            const createdIndex = this.connections.push({
                id: createdID,
                source: sourceNodeID,
                out: sourceIndex,
                dest: destNodeID,
                in: destIndex,
            });

            this.cacheConnections[createdID] = createdIndex - 1;
            this.lastCreatedID++;
        },
        removeConnection(connectionID) {
            if (this.cacheConnections[connectionID] == null) {
                throw new Error("Connection with this ID does not exist");
            }

            const connection =
                this.connections[this.cacheConnections[connectionID]];

            const sourceNode =
                this.$refs.nodeInstance[this.cacheNodes[connection.source]];
            sourceNode.connectedOutputs[connection.out] = false;

            const destNode =
                this.$refs.nodeInstance[this.cacheNodes[connection.dest]];
            destNode.connectedInputs[connection.in] = false;

            this.connections.splice(this.cacheConnections[connectionID], 1);
            this.cacheConnections = {};
            for (let i = 0; i < this.connections.length; i++) {
                this.cacheConnections[this.connections[i].id] = i;
            }
        },
        keyHandler(e) {
            if (e.code == "Backspace" || e.code == "Delete") {
                if (this.focusedElement) {
                    if ($(document.activeElement).is(":input")) {
                        return;
                    }

                    if (
                        $(this.focusedElement).closest(
                            ".flowgraph-node-container"
                        ).length > 0
                    ) {
                        this.removeNode(
                            this.focusedElement.__vueParentComponent.proxy
                                .$props.id
                        );
                    } else if (
                        $(this.focusedElement).closest(".flowgraph-connection")
                            .length > 0
                    ) {
                        this.removeConnection(
                            this.focusedElement.__vueParentComponent.proxy
                                .$props.id
                        );
                    }
                }
            }
        },
        dragNodeBeginHandler(e) {
            if (
                $(e.target)
                    .closest(".flowgraph-node-io.circle")
                    .closest(".flowgraph-node-io-row.left").length > 0
            ) {
                this.dragMode = "io.input";
            } else if (
                $(e.target)
                    .closest(".flowgraph-node-io.circle")
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

            if (
                $(e.target).closest(".flowgraph-context-container").length > 0
            ) {
            } else {
                this.$refs.ctxMenu.close();
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
                case "io.input": {
                    this.selectedElement = $(e.target)
                        .closest(".flowgraph-node-io.circle")
                        .get(0);

                    const destIndex = Number($(e.target).attr("data-io-index"));

                    const connections = Object.values(this.connections).filter(
                        (element) =>
                            element.dest ==
                                this.selectedElement.__vueParentComponent.proxy
                                    .$props.id && element.in == destIndex
                    );

                    if (connections.length != 0) {
                        for (const connection of Object.values(connections)) {
                            this.removeConnection(connection.id);
                        }
                    }
                    break;
                }
                case "io.output": {
                    this.selectedElement = $(e.target)
                        .closest(".flowgraph-node-io.circle")
                        .get(0);

                    const sourceIndex = Number(
                        $(e.target).attr("data-io-index")
                    );

                    const posX =
                        this.selectedElement.__vueParentComponent.proxy
                            .absolutePosition[0] +
                        this.selectedElement.offsetLeft +
                        this.selectedElement.offsetWidth / 2;
                    const posY =
                        this.selectedElement.__vueParentComponent.proxy
                            .absolutePosition[1] +
                        this.selectedElement.offsetTop +
                        this.selectedElement.offsetHeight / 2 +
                        2;

                    this.selectedElement.__vueParentComponent.proxy.$data.connectedOutputs[
                        sourceIndex
                    ] = true;

                    this.previewConnection = true;
                    this.previewPosition = [posX, posY];
                    this.dummyPosition = [posX - 50, posY];
                    break;
                }
                case "connection": {
                    this.selectedElement = $(e.target)
                        .closest(".flowgraph-connection")
                        .get(0);
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
                case "io.output": {
                    const sourceIndex = Number(
                        $(this.selectedElement).attr("data-io-index")
                    );

                    if (
                        $(e.target).closest(".flowgraph-node-io-row.left")
                            .length > 0
                    ) {
                        const destElement = $(e.target)
                            .closest(".flowgraph-node-io-row.left")
                            .find(".flowgraph-node-io.circle")
                            .get(0);

                        const destIndex = Number(
                            $(e.target)
                                .closest(".flowgraph-node-io-row.left")
                                .find(".flowgraph-node-io.circle")
                                .attr("data-io-index")
                        );

                        if (
                            this.isTypeCheck(
                                this.selectedElement.__vueParentComponent,
                                sourceIndex,
                                destElement.__vueParentComponent,
                                destIndex
                            )
                        ) {
                            const connection = Object.values(
                                this.connections
                            ).find(
                                (element) =>
                                    element.dest ==
                                        destElement.__vueParentComponent.proxy
                                            .$props.id &&
                                    element.in == destIndex
                            );

                            if (connection) {
                                this.removeConnection(connection.id);
                            }

                            this.addConnection(
                                this.selectedElement.__vueParentComponent.proxy
                                    .$props.id,
                                sourceIndex,
                                destElement.__vueParentComponent.proxy.$props
                                    .id,
                                destIndex
                            );
                        }
                    }

                    const connections = Object.values(this.connections).filter(
                        (element) =>
                            element.source ==
                                this.selectedElement.__vueParentComponent.proxy
                                    .$props.id && element.out == sourceIndex
                    );

                    if (connections.length == 0) {
                        this.selectedElement.__vueParentComponent.proxy.$data.connectedOutputs[
                            sourceIndex
                        ] = false;
                    }

                    this.previewConnection = false;
                    break;
                }
                case "connection": {
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
                    const posX =
                        this.canvasPosition[0] + relativeX / this.zoomScale;
                    const posY =
                        this.canvasPosition[1] + relativeY / this.zoomScale;

                    this.canvasPosition = [posX, posY];
                    break;
                }
                case "node": {
                    if (this.selectedElement) {
                        const node =
                            this.nodes[
                                this.cacheNodes[
                                    this.selectedElement.__vueParentComponent
                                        .proxy.$props.id
                                ]
                            ];

                        const posX =
                            node.position[0] + relativeX / this.zoomScale;
                        const posY =
                            node.position[1] + relativeY / this.zoomScale;

                        this.nodes[
                            this.cacheNodes[
                                this.selectedElement.__vueParentComponent.proxy.$props.id
                            ]
                        ].position = [posX, posY];
                    }
                    break;
                }
                case "io.output": {
                    if (this.selectedElement) {
                        const destPosX =
                            this.dummyPosition[0] + relativeX / this.zoomScale;
                        const destPosY =
                            this.dummyPosition[1] + relativeY / this.zoomScale;

                        this.dummyPosition = [destPosX, destPosY];
                    }
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

                this.zoomScale = parseFloat(this.zoomScale.toFixed(2));

                $(this.$el)
                    .find(".flowgraph-bg-container")
                    .get(0)
                    .style.setProperty(
                        "--grid-size",
                        `${this.zoomScale * 40}px`
                    );
            }
        },
        isTypeCheck(sourceNode, sourceIndex, destNode, destIndex) {
            return (
                sourceNode.proxy.$props.outputs[sourceIndex].type ==
                destNode.proxy.$props.inputs[destIndex].type
            );
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
    transform-origin: center;
}

.flowgraph-vectorlayer {
    display: block;
    width: 100%;
    height: 100%;
    position: absolute;
    filter: drop-shadow(0 0px 5px rgba(0, 0, 0, 0.5));
}

.flowgraph-dummy-pointer {
    display: block;
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    width: 12px;
    height: 12px;
    opacity: 0;
    transform-origin: center;
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