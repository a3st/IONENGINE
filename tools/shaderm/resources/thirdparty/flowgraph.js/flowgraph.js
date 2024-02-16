import 'https://code.jquery.com/jquery-3.6.0.min.js';
const $ = window.$;

export default class FlowGraph {
    constructor(rootNode, canvasOptions = { "width": 4096, "height": 4096 }) {
        this.rootNode = rootNode;
        this.canvasOptions = canvasOptions;

        this.nodeIndex = 0;
        this.nodes = {};

        this.connectionIndex = 0;
        this.connections = {};

        this.selectedElement = null;
        this.dragMode = "none";
        this.zoomScale = 1.0;

        this.lastClientX = 0;
        this.lastClientY = 0;
        this.lastZoomScale = 1.0;
    }

    start() {
        $(this.rootNode).addClass('flowgraph-container');
        $(this.rootNode).append(`
            <div class="flowgraph-bg-container" 
                style="width: ${this.rootNode.style.width}; height: ${this.rootNode.style.height};"></div>
            <div class="flowgraph-canvas"
                style="width: ${this.canvasOptions.width}px; height: ${this.canvasOptions.height}px;">

                <svg class="flowgraph-vectorlayer" 
                    style="width: ${this.canvasOptions.width}px; height: ${this.canvasOptions.height}px;" xmlns="http://www.w3.org/2000/svg"></svg>
            </div>
        `);

        this.rootNode.addEventListener('mousewheel', this.#zoomEventHandler.bind(this));
        this.rootNode.addEventListener('mousedown', this.#dragNodeBeginHandler.bind(this));
        this.rootNode.addEventListener('mouseup', this.#dragNodeEndHandler.bind(this));
        this.rootNode.addEventListener('mousemove', this.#moveNodeHandler.bind(this));

        $(this.rootNode).children('.flowgraph-canvas').css('transform', 'translate(0px, 0px)');
    }

    #dragNodeBeginHandler(e) {
        if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.left').length > 0) {
            this.dragMode = "io.input";
        } else if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.right').length > 0) {
            this.dragMode = "io.output";
        } else if ($(e.target).closest('.flowgraph-node-container').length > 0) {
            this.dragMode = "node";
        } else if ($(e.target).closest('.flowgraph-connection').length > 0) {
            this.dragMode = "connection";
        } else if ($(e.target).closest('.flowgraph-canvas').length > 0) {
            this.dragMode = "canvas";
        }

        switch (this.dragMode) {
            case "io.input": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-io')
                    .get(0);

                const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex))

                if (connection) {
                    $(`#node_${destNode}_in_${inIndex}`)
                        .removeClass('connected')
                        .get(0);
                }

                const elementMatrix = $(this.selectedElement)
                    .closest('.flowgraph-node-container')
                    .css('transform')
                    .match(/-?[\d\.]+/g);

                const posX = Number(elementMatrix[4]) + this.selectedElement.offsetLeft;
                const posY = Number(elementMatrix[5]) + this.selectedElement.offsetTop;

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .append(`
                        <div id="drag-io-dummy" class="flowgraph-io circle" style="pointer-events: none; position: absolute; opacity: 0; transform: translate(${posX}px, ${posY}px)"></div>
                    `);
                break;
            }
            case "io.output": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-io')
                    .get(0);

                const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                this.#addPreviewConnection(sourceNode, outIndex);

                const elementMatrix = $(this.selectedElement)
                    .closest('.flowgraph-node-container')
                    .css('transform')
                    .match(/-?[\d\.]+/g);

                const posX = Number(elementMatrix[4]) + this.selectedElement.offsetLeft;
                const posY = Number(elementMatrix[5]) + this.selectedElement.offsetTop;

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .append(`
                        <div id="drag-io-dummy" class="flowgraph-io circle" style="pointer-events: none; position: absolute; opacity: 0; transform: translate(${posX}px, ${posY}px)"></div>
                    `)
                break;
            }
            case "node": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-node-container')
                    .get(0);

                $(this.selectedElement).addClass('selected');
                break;
            }
            case "canvas": {
                console.log("Start drag canvas");
                break;
            }
            case "connection": {
                console.log("Select connection");
                break;
            }
        }
    }

    #dragNodeEndHandler(e) {
        switch (this.dragMode) {
            case "io.input": {
                if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.left').length > 0) {
                    let destElement = $(e.target).closest('.flowgraph-io')[0];
                    const [destNode, inIndex] = destElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        $(`#node_${destNode}_in_${inIndex}`)
                            .addClass('connected')
                            .get(0);

                        this.#updateNode(destNode);
                    } else {
                        const [selectedNode, selectedIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                        const connection = Object.values(this.connections).find(value => (value.dest == selectedNode && value.in == selectedIndex));

                        this.#updateConnection(connection.id, connection.source, connection.out, destNode, inIndex);
                    }
                } else {
                    const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        this.removeConnection(connection.id);
                    }
                }

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .children('#drag-io-dummy')
                    .remove();
                break;
            }
            case "io.output": {
                if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.left').length > 0) {
                    let destElement = $(e.target).closest('.flowgraph-io')[0];

                    const [destNode, inIndex] = destElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        const connections = Object.values(this.connections).filter(
                            value => (value.source == this.connections[connection.id].source && value.out == this.connections[connection.id].out));
                        if(connections.length == 1) {
                            $(`#node_${this.connections[connection.id].source}_out_${this.connections[connection.id].out}`)
                            .removeClass('connected')
                            .get(0);
                        }

                        this.#updateConnection(connection.id, sourceNode, outIndex, destNode, inIndex);
                    } else {
                        this.addConnection(sourceNode, outIndex, destNode, inIndex);
                    }
                } else {
                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));

                    $(`#node_${sourceNode}_out_${outIndex}`)
                        .removeClass('connected')
                        .get(0);
                }

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .children('#drag-io-dummy')
                    .remove();

                const vectorLayer = $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .children('.flowgraph-vectorlayer')
                    .get(0);

                let svgElement = vectorLayer.getElementById(`connection_preview`);
                svgElement.remove();
                break;
            }
        }

        console.log("Stop drag " + this.dragMode);
        this.dragMode = "none";
        if (this.selectedElement) {
            $(this.selectedElement).removeClass('selected');
            this.selectedElement = null;
        }
    }

    #updateNode(nodeId) {
        const connections = Object.values(this.connections).filter(value => (value.source == nodeId || value.dest == nodeId));

        for (const [_, value] of Object.entries(connections)) {
            const sourceElement = $(`#node_${this.connections[value.id].source}_out_${this.connections[value.id].out}`).get(0);
            const sourceMatrix = $(`#node_${this.connections[value.id].source}`)
                .css('transform')
                .match(/-?[\d\.]+/g);

            const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
            const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

            const destElement = $(`#node_${this.connections[value.id].dest}_in_${this.connections[value.id].in}`).get(0);
            const destMatrix = $(`#node_${this.connections[value.id].dest}`)
                .css('transform')
                .match(/-?[\d\.]+/g);

            const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
            const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

            this.#updateConnectionPosition(value.id, sourceX, sourceY, destX, destY);
        }
    }

    #moveNodeHandler(e) {
        const relativeClientX = e.clientX - this.lastClientX;
        const relativeClientY = e.clientY - this.lastClientY;
        this.lastClientX = e.clientX;
        this.lastClientY = e.clientY;

        switch (this.dragMode) {
            case "canvas": {
                const posMatrix = $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .css('transform')
                    .match(/-?[\d\.]+/g);

                const posX = Number(posMatrix[4]) + relativeClientX / this.zoomScale;
                const posY = Number(posMatrix[5]) + relativeClientY / this.zoomScale;

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .css('transform', `translate(${posX}px, ${posY}px) scale(${this.zoomScale})`);
                break;
            }
            case "node": {
                if (this.selectedElement) {
                    const posMatrix = $(this.selectedElement)
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeClientX / this.zoomScale;
                    const posY = Number(posMatrix[5]) + relativeClientY / this.zoomScale;

                    $(this.selectedElement).css('transform', `translate(${posX}px, ${posY}px)`);

                    const nodeId = Number(this.selectedElement.id.match(/-?[\d]/g)[0]);
                    this.#updateNode(nodeId);
                }
                break;
            }
            case "io.input": {
                if (this.selectedElement) {
                    const posMatrix = $(this.rootNode)
                        .children('.flowgraph-canvas')
                        .children('#drag-io-dummy')
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeClientX / this.zoomScale;
                    const posY = Number(posMatrix[5]) + relativeClientY / this.zoomScale;

                    $(this.rootNode)
                        .children('.flowgraph-canvas')
                        .children('#drag-io-dummy')
                        .css('transform', `translate(${posX}px, ${posY}px)`);

                    const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        const sourceElement = $(`#node_${this.connections[connection.id].source}_out_${this.connections[connection.id].out}`).get(0);
                        const sourceMatrix = $(`#node_${this.connections[connection.id].source}`)
                            .css('transform')
                            .match(/-?[\d\.]+/g);

                        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
                        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;
                        const destX = posX;
                        const destY = posY;

                        this.#updateConnectionPosition(connection.id, sourceX, sourceY, destX, destY);
                    }
                }
                break;
            }
            case "io.output": {
                if (this.selectedElement) {
                    const posMatrix = $(this.rootNode)
                        .children('.flowgraph-canvas')
                        .children('#drag-io-dummy')
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeClientX / this.zoomScale;
                    const posY = Number(posMatrix[5]) + relativeClientY / this.zoomScale;

                    $(this.rootNode)
                        .children('.flowgraph-canvas')
                        .children('#drag-io-dummy')
                        .css('transform', `translate(${posX}px, ${posY}px)`);

                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));

                    const sourceElement = $(`#node_${sourceNode}_out_${outIndex}`).get(0);
                    const sourceMatrix = $(`#node_${sourceNode}`)
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
                    const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;
                    const destX = posX;
                    const destY = posY;

                    this.#updateConnectionPreviewPosition(sourceX, sourceY, destX, destY);
                }
                break;
            }
        }
    }

    #zoomEventHandler(e) {
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

        const posMatrix = $(this.rootNode)
            .children('.flowgraph-canvas')
            .css('transform')
            .match(/-?[\d\.]+/g);

        const posX = (Number(posMatrix[4]) / this.lastZoomScale) * this.zoomScale;
        const posY = (Number(posMatrix[5]) / this.lastZoomScale) * this.zoomScale;

        this.lastZoomScale = this.zoomScale;

        $(this.rootNode)
            .children('.flowgraph-canvas')
            .css('transform', `translate(${posX}px, ${posY}px) scale(${this.zoomScale})`);
    }

    removeConnection(connectionId) {
        const connections = Object.values(this.connections).filter(
            value => (value.source == this.connections[connectionId].source && value.out == this.connections[connectionId].out));
        if(connections.length == 1) {
            $(`#node_${this.connections[connectionId].source}_out_${this.connections[connectionId].out}`)
            .removeClass('connected')
            .get(0);
        }

        $(`#node_${this.connections[connectionId].dest}_in_${this.connections[connectionId].in}`)
            .removeClass('connected')
            .get(0);

        const vectorLayer = $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .get(0);

        let svgElement = vectorLayer.getElementById(`connection_${connectionId}`);
        svgElement.remove();

        delete this.connections[connectionId];
    }

    #updateConnectionPosition(connectionId, sourceX, sourceY, destX, destY) {
        const controlX = (sourceX + destX) / 2;
        const controlY = (sourceY + destY) / 2;

        const vectorLayer = $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .get(0);

        let svgElement = vectorLayer.getElementById(`connection_${connectionId}`);
        svgElement.setAttribute('d', `M ${sourceX} ${sourceY} Q ${controlX} ${controlY} ${destX} ${destY}`);
    }

    #addPreviewConnection(sourceNode, outIndex) {
        $(`#node_${sourceNode}_out_${outIndex}`)
            .addClass('connected')
            .get(0);

        let svgElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
        svgElement.classList.add('flowgraph-connection');
        svgElement.setAttribute('id', `connection_preview`);
        svgElement.setAttribute('stroke', 'rgb(42, 143, 42)');
        svgElement.setAttribute('stroke-width', '8');
        svgElement.setAttribute('fill', 'transparent');

        $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .append(svgElement);
    }

    #updateConnectionPreviewPosition(sourceX, sourceY, destX, destY) {
        const controlX = (sourceX + destX) / 2;
        const controlY = (sourceY + destY) / 2;

        const vectorLayer = $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .get(0);
        let svgElement = vectorLayer.getElementById(`connection_preview`);
        svgElement.setAttribute('d', `M ${sourceX} ${sourceY} Q ${controlX} ${controlY} ${destX} ${destY}`);
    }

    #updateConnection(connectionId, sourceNodeId, outIndex, destNodeId, inIndex) {
        const sourceElement = $(`#node_${sourceNodeId}_out_${outIndex}`)
            .addClass('connected')
            .get(0);
        const sourceMatrix = $(`#node_${sourceNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

        const destElement = $(`#node_${destNodeId}_in_${inIndex}`)
            .addClass('connected')
            .get(0);
        const destMatrix = $(`#node_${destNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

        this.connections[connectionId] = {
            "id": connectionId,
            "source": sourceNodeId,
            "out": outIndex,
            "dest": destNodeId,
            "in": inIndex
        };

        this.#updateConnectionPosition(connectionId, sourceX, sourceY, destX, destY);
    }

    addConnection(sourceNodeId, outIndex, destNodeId, inIndex) {
        const sourceElement = $(`#node_${sourceNodeId}_out_${outIndex}`)
            .addClass('connected')
            .get(0);
        const sourceMatrix = $(`#node_${sourceNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

        const destElement = $(`#node_${destNodeId}_in_${inIndex}`)
            .addClass('connected')
            .get(0);
        const destMatrix = $(`#node_${destNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

        let svgElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
        svgElement.classList.add('flowgraph-connection');
        svgElement.setAttribute('id', `connection_${this.connectionIndex}`);
        svgElement.setAttribute('stroke', 'rgb(42, 143, 42)');
        svgElement.setAttribute('stroke-width', '8');
        svgElement.setAttribute('fill', 'transparent');

        $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .append(svgElement);

        this.connections[this.connectionIndex] = {
            "id": this.connectionIndex,
            "source": sourceNodeId,
            "out": outIndex,
            "dest": destNodeId,
            "in": inIndex
        };

        this.#updateConnectionPosition(this.connectionIndex, sourceX, sourceY, destX, destY);

        this.connectionIndex += 1;
        return this.connections[this.connectionIndex - 1];
    }

    addNode(x, y, inputs, outputs, headerHTML) {
        let inputsHTML = "";
        let inputIndex = 0;
        let inputsData = {};

        for (const input of inputs) {
            inputsHTML += `
                <div class="flowgraph-io-row left">
                    <div id="node_${this.nodeIndex}_in_${inputIndex}" class="flowgraph-io circle"></div>
                    <span style="color: white;">${input.name} (${input.type})</span>
                </div>
            `;

            inputsData[inputIndex] = {
                "name": input.name, "type": input.type
            }

            inputIndex += 1;
        }

        let outputsHTML = "";
        let outputIndex = 0;
        let outputsData = {};

        for (const output of outputs) {
            outputsHTML += `
                <div class="flowgraph-io-row right">
                    <span style="color: white;">${output.name} (${output.type})</span>
                    <div id="node_${this.nodeIndex}_out_${outputIndex}" class="flowgraph-io circle"></div>
                </div>
            `;

            outputsData[outputIndex] = {
                "name": output.name, "type": output.type
            }

            outputIndex += 1;
        }

        $(this.rootNode).children('.flowgraph-canvas').append(`
            <div id="node_${this.nodeIndex}" class="flowgraph-node-container" style="width: 350px; height: 200px;"> 
                <div class="flowgraph-node-root">
                    <div class="flowgraph-node-header">
                        ${headerHTML}
                    </div>

                    <div class="flowgraph-node-main">
                        <div class="flowgraph-io-list">
                            ${inputsHTML}
                        </div>

                        <div class="flowgraph-io-list">
                            ${outputsHTML}
                        </div>
                    <div>
                </div>
            </div>
        `);

        $(`#node_${this.nodeIndex}`).css('transform', `translate(${x}px, ${y}px)`);

        this.nodes[this.nodeIndex] = {
            "id": this.nodeIndex,
            "position": [x, y],
            "inputs": inputsData,
            "outputs": outputsData
        };

        this.nodeIndex += 1;
        return this.nodes[this.nodeIndex - 1];
    }
}