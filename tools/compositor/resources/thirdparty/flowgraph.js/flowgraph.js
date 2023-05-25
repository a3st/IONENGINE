import 'https://code.jquery.com/jquery-3.6.0.min.js';
const $ = window.$;

export default class FlowGraph {
    constructor(rootNode, canvasOptions = { "width": 4096, "height": 4096 }) {
        this.rootNode = rootNode;
        this.canvasOptions = canvasOptions;

        this.nodeIndex = 0;
        this.nodes = { };

        this.connectionIndex = 0;
        this.connections = { };

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

        this.rootNode.addEventListener('mousewheel', this.zoomEventHandler.bind(this));
        this.rootNode.addEventListener('mousedown', this.dragNodeBeginHandler.bind(this));
        this.rootNode.addEventListener('mouseup', this.dragNodeEndHandler.bind(this));
        this.rootNode.addEventListener('mousemove', this.moveNodeHandler.bind(this));

        $(this.rootNode).children('.flowgraph-canvas').css('transform', 'translate(0px, 0px)');
    }

    dragNodeBeginHandler(e) {
        if($(e.target).closest('.flowgraph-io').length > 0) {
            this.dragMode = "io";
        } else if($(e.target).closest('.flowgraph-node-container').length > 0) {
            this.dragMode = "node";
        } else if($(e.target).closest('.flowgraph-connection').length > 0) {
            this.dragMode = "connection";
        } else if($(e.target).closest('.flowgraph-canvas').length > 0) {
            this.dragMode = "canvas";
        }

        switch(this.dragMode) {
            case "io": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-io')
                    .get(0);

                console.log("Start drag io");
            } break;

            case "node": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-node-container')
                    .get(0);

                $(this.selectedElement).addClass('selected');

                console.log("Start drag node");
            } break;

            default: { } break;

            case "canvas": {
                console.log("Start drag canvas");
            } break;

            case "connection": {
                console.log("Select connection");
            } break;
        }

        // console.log(e.clientX, e.clientY)
    }

    dragNodeEndHandler(e) {
        console.log("Stop drag " + this.dragMode);
        this.dragMode = "none";
        if(this.selectedElement) {
            $(this.selectedElement).removeClass('selected');
            this.selectedElement = null;
        }
    }

    moveNodeHandler(e) {
        const relativeClientX = e.clientX - this.lastClientX;
        const relativeClientY = e.clientY - this.lastClientY;
        this.lastClientX = e.clientX;
        this.lastClientY = e.clientY;

        switch(this.dragMode) {
            case "canvas": {
                const posMatrix = $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .css('transform')
                    .match(/-?[\d\.]+/g);

                const posX = Number(posMatrix[4]) + relativeClientX;
                const posY = Number(posMatrix[5]) + relativeClientY;

                $(this.rootNode)
                    .children('.flowgraph-canvas')
                    .css('transform', `translate(${posX}px, ${posY}px) scale(${this.zoomScale})`);
            } break;
            case "node": {
                if(this.selectedElement) {
                    const posMatrix = $(this.selectedElement)
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeClientX;
                    const posY = Number(posMatrix[5]) + relativeClientY;

                    $(this.selectedElement).css('transform', `translate(${posX}px, ${posY}px) scale(${this.zoomScale})`);

                    const nodeId = Number(this.selectedElement.id.match(/-?[\d]/g)[0]);
                    
                    const connections = Object.fromEntries(
                        Object.entries(this.connections).filter((key, value) => 
                            (this.connections[value].source == nodeId || this.connections[value].dest == nodeId))
                    );
                    
                    for(const [_, value] of Object.entries(connections)) {
                        const connectionId = value.id;

                        const sourceElement = $(`#node_${this.connections[connectionId].source}_out_${this.connections[connectionId].out}`).get(0);
                        const sourceMatrix = $(`#node_${this.connections[connectionId].source}`)
                            .css('transform')
                            .match(/-?[\d\.]+/g);

                        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
                        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

                        const destElement = $(`#node_${this.connections[connectionId].dest}_in_${this.connections[connectionId].in}`).get(0);
                        const destMatrix = $(`#node_${this.connections[connectionId].dest}`)
                            .css('transform')
                            .match(/-?[\d\.]+/g);

                        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
                        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

                        const controlX = (sourceX + destX) / 2;
                        const controlY = (sourceY + destY) / 2 - (sourceY - destY);

                        const vectorLayer = $(this.rootNode)
                            .children('.flowgraph-canvas')
                            .children('.flowgraph-vectorlayer')
                            .get(0);

                        let svgElement = vectorLayer.getElementById(`connection_${connectionId}`);
                        svgElement.setAttribute('d', `M ${sourceX} ${sourceY} Q ${controlX} ${controlY} ${destX} ${destY}`);
                    }
                }
            } break;
            case "io": {
                if(this.selectedElement) {
                    const [nodeId, ioIndex] = this.selectedElement.id.match(/-?[\d]/g).map((x) => (parseInt(x)));

                    this.connections.find

                }
            } break;
        }
    }

    zoomEventHandler(e) {
        e.preventDefault();

        if(e.deltaY > 0) {
            if(this.zoomScale > 0.4) {
                this.zoomScale -= 0.05;
            }
        } else {
            if(this.zoomScale < 1.3) {
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

    addConnection(sourceNode, outIndex, destNode, inIndex) {
        const sourceElement = $(`#node_${sourceNode.id}_out_${outIndex}`)
            .addClass('connected')
            .get(0);
        const sourceMatrix = $(`#node_${sourceNode.id}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

        const destElement = $(`#node_${destNode.id}_in_${inIndex}`)
            .addClass('connected')
            .get(0);
        const destMatrix = $(`#node_${destNode.id}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

        const controlX = (sourceX + destX) / 2;
        const controlY = (sourceY + destY) / 2 - (sourceY - destY);

        let svgElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
        svgElement.classList.add('flowgraph-connection');
        svgElement.setAttribute('id', `connection_${this.connectionIndex}`);
        svgElement.setAttribute('d', `M ${sourceX} ${sourceY} Q ${controlX} ${controlY} ${destX} ${destY}`);
        svgElement.setAttribute('stroke', 'rgb(42, 143, 42)');
        svgElement.setAttribute('stroke-width', '8');
        svgElement.setAttribute('fill', 'transparent');

        $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .append(svgElement);

        this.connections[this.connectionIndex] = {
            "id" : this.connectionIndex,
            "source" : sourceNode.id,
            "out" : outIndex,
            "dest" : destNode.id,
            "in" : inIndex
        };

        this.connectionIndex += 1;

        return this.connections[this.connectionIndex - 1];
    }

    addNode(x, y, inputs, outputs, headerHTML) {
        let inputsHTML = "";
        let inputIndex = 0;
        let inputsData = {};

        for(const input of inputs) {
            inputsHTML += `
                <div class="flowgraph-io-row left">
                    <div id="node_${this.nodeIndex}_in_${inputIndex}" class="flowgraph-io circle"></div>
                    <span style="color: white;">${input.name} (${input.type})</span>
                </div>
            `;

            inputsData[inputIndex] = { 
                "name" : input.name, "type" : input.type
            }

            inputIndex += 1;
        }

        let outputsHTML = "";
        let outputIndex = 0;
        let outputsData = {};

        for(const output of outputs) {
            outputsHTML += `
                <div class="flowgraph-io-row right">
                    <span style="color: white;">${output.name} (${output.type})</span>
                    <div id="node_${this.nodeIndex}_out_${outputIndex}" class="flowgraph-io circle"></div>
                </div>
            `;

            outputsData[outputIndex] = { 
                "name" : output.name, "type" : output.type
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
            "id" : this.nodeIndex,
            "position" : [x, y],
            "inputs" : inputsData,
            "outputs" : outputsData
        };

        this.nodeIndex += 1;

        return this.nodes[this.nodeIndex - 1];
    }
}