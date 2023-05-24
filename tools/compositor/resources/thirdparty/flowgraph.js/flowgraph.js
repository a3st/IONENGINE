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

        this.selectedNode = null;
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
                style="width: ${this.canvasOptions.width}px; height: ${this.canvasOptions.height}px;"></div>
        `);

        this.rootNode.addEventListener('mousewheel', this.zoom_event_handler.bind(this));
        this.rootNode.addEventListener('mousedown', this.drag_node_begin_handler.bind(this));
        this.rootNode.addEventListener('mouseup', this.drag_node_end_handler.bind(this));
        this.rootNode.addEventListener('mousemove', this.move_node_handler.bind(this));

        $(this.rootNode).children('.flowgraph-canvas').css('transform', 'translate(0px, 0px)');
    }

    drag_node_begin_handler(e) {
        if($(e.target).closest('.flowgraph-io').length > 0) {
            this.dragMode = "io";
        } else if($(e.target).closest('.flowgraph-node-container').length > 0) {
            this.dragMode = "node";
        } else if($(e.target).closest('.flowgraph-canvas').length > 0) {
            this.dragMode = "canvas";
        }

        switch(this.dragMode) {
            case "io": {
                /*const target = $(e.target).closest('.flowgraph-io').get(0);

                $(this.rootNode).append(`
                        <svg height="800" width="800">
                            <path d="M " stroke="black" fill="transparent"/>
                        </svg>
                    `);
                */
                console.log("Start drag io");
            } break;

            case "node": {
                this.selectedNode = $(e.target)
                    .closest('.flowgraph-node-container')
                    .get(0);

                $(this.selectedNode).addClass('selected');

                console.log("Start drag node");
            } break;

            default: { } break;

            case "canvas": {
                console.log("Start drag canvas");
            } break;
        }

        console.log(e.clientX, e.clientY)
    }

    drag_node_end_handler(e) {
        console.log("Stop drag " + this.dragMode);
        this.dragMode = "none";
        if(this.selectedNode) {
            $(this.selectedNode).removeClass('selected');
            this.selectedNode = null;
        }
    }

    move_node_handler(e) {
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
                if(this.selectedNode) {
                    const posMatrix = $(this.selectedNode)
                        .css('transform')
                        .match(/-?[\d\.]+/g);

                    const posX = Number(posMatrix[4]) + relativeClientX;
                    const posY = Number(posMatrix[5]) + relativeClientY;

                    $(this.selectedNode).css('transform', `translate(${posX}px, ${posY}px)`);
                }
            } break;
        }
    }

    zoom_event_handler(e) {
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

    add_connection(sourceNode, outIndex, destNode, inIndex) {

        const element = $(`#node_${sourceNode.id}_out_${outIndex}`);

        const startX = element.offset().top;
        const startY = element.offset().left;

        console.log(startX, startY)
        const rect = element.get(0).getBoundingClientRect()
        console.log(rect)

        $(this.rootNode).children('.flowgraph-canvas').append(`
            <svg width="800" height="800" xmlns="http://www.w3.org/2000/svg">
                <path style="z-index: -1;" d="M ${rect.right} ${rect.bottom} L 40 40" stroke="green" stroke-width="8" fill="transparent"/>
            </svg>
        `);
    }

    add_node(x, y, inputs, outputs, headerHTML) {
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
                <div id="node_${this.nodeIndex}_out_${outputIndex}" class="flowgraph-io-row right">
                    <span style="color: white;">${output.name} (${output.type})</span>
                    <div class="flowgraph-io circle"></div>
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