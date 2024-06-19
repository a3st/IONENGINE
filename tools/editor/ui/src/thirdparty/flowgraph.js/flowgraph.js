// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

import $ from 'jquery'

export default class FlowGraph {
    constructor(rootNode, width, height, canvasOptions = { "width": 8096, "height": 8096 }) {
        this.rootNode = rootNode;
        this.width = width;
        this.height = height;
        this.canvasOptions = canvasOptions;

        this.nodes = [];
        this.nodesCache = {};
        this.connections = [];
        this.connectionsCache = {};

        this.focusedElement = null;
        this.selectedElement = null;
        this.dragMode = "none";
        this.zoomScale = 1.0;

        this.relativeCanvasX = canvasOptions.width / 2;
        this.relativeCanvasY = canvasOptions.height / 2;

        this.lastClientX = 0;
        this.lastClientY = 0;

        this.groups = {};

        this.lastCreatedId = 0;
    }

    /**
     * Add new element to context menu
     * @param {string} group - Group of elements
     * @param {string} item - Function name
     * @param {*} callback - Function callback
     */
    addContextItem(group, item, callback) {
        if (!(group in this.groups)) {
            this.groups[group] = [];
        }
        this.groups[group].push({ 'item': item, 'callback': callback });
    }

    resize(width, height) {
        $(this.rootNode)
            .css('width', width)
            .css('height', height);
        $(this.rootNode).find('.flowgraph-bg-container')
            .css('width', width)
            .css('height', height);

        this.width = width;
        this.height = height;
    }

    start(options = {}) {
        $(this.rootNode)
            .addClass('flowgraph-container')
            .css('width', this.width)
            .css('height', this.height);
        $(this.rootNode).append(`
            <div class="flowgraph-bg-container" 
                style="width: ${this.rootNode.style.width}; height: ${this.rootNode.style.height};"></div>
            <div class="flowgraph-canvas"
                style="width: ${this.canvasOptions.width}px; height: ${this.canvasOptions.height}px;">

                <svg class="flowgraph-vectorlayer" 
                    style="width: ${this.canvasOptions.width}px; height: ${this.canvasOptions.height}px;" xmlns="http://www.w3.org/2000/svg"></svg>
            </div>
        `);
        $(this.rootNode).append(`
            <div class="flowgraph-context-container" style="display: none;">
                <div class="flowgraph-context-search">
                    <object data="images/search.svg" width="16" height="16"> </object>
                    <input type="text" placeholder="Type to search" search />
                </div>
                <div class="flowgraph-context-main"></div>
            </div>
        `);
        $('.flowgraph-context-search input').bind('input', e => { this.#renderContextMenu(); });

        $(this.rootNode)
            .find('.flowgraph-bg-container')
            .get(0).style.setProperty('--grid-size', `${this.zoomScale * 40}px`);

        this.rootNode.addEventListener('mousewheel', this.#zoomEventHandler.bind(this));
        this.rootNode.addEventListener('mousedown', event => {
            if (event.which == 1) {
                this.#dragNodeBeginHandler(event);
            } else if (event.which == 3) {
                this.#rightClickHandler(event);
            }
        });
        this.rootNode.addEventListener('mouseup', this.#dragNodeEndHandler.bind(this));
        this.rootNode.addEventListener('mousemove', this.#moveNodeHandler.bind(this));
        this.rootNode.addEventListener('contextmenu', event => event.preventDefault());

        document.addEventListener('keydown', this.#keyHandler.bind(this));

        $(this.rootNode)
            .children('.flowgraph-canvas')
            .css('transform', `translate(${-this.relativeCanvasX}px, ${-this.relativeCanvasY}px)`);

        if(options['valueChanged'] != null) {
            this.rootNode.addEventListener("flowgraph:value-changed", options['valueChanged']);
        } else {
            this.rootNode.addEventListener("flowgraph:value-changed", e => { });
        }
    }

    #renderContextMenu() {
        const contextMenuElement = $(this.rootNode).find('.flowgraph-context-main')
            .empty();

        const canvasMatrix = $(this.rootNode).find('.flowgraph-canvas')
            .css('transform')
            .match(/-?[\d\.]+/g);

        const posX = this.lastClientX + (-this.relativeCanvasX - Number(canvasMatrix[4]));
        const posY = this.lastClientY + (-this.relativeCanvasY - Number(canvasMatrix[5]));

        const searchPattern = $(this.rootNode).find('.flowgraph-context-search input').get(0).value;
        let filteredGroups;
        if (searchPattern.length == 0) {
            filteredGroups = Object.entries(this.groups);
        } else {
            filteredGroups = Object.entries(this.groups).filter(
                (key, value) => (
                    Object.values(this.groups[searchPattern]).filter(
                        value => String(value.item).includes(searchPattern)
                    ).length > 0 | String(key).includes(searchPattern)
                )
            );
        }

        for (const [group, data] of filteredGroups) {
            contextMenuElement
                .append(`<label>${group}</label>`)
                .append(`<div class="flowgraph-context-group"></div>`);

            let groupMenuElement = $(this.rootNode).find('.flowgraph-context-group').last();
            for (const item of data) {
                groupMenuElement.append(`<button>${item.item}</button>`);
                $(this.rootNode).find('.flowgraph-context-group button')
                    .last()
                    .bind('click', e => {
                        item.callback({"posX": posX, "posY": posY});
                        $(this.rootNode).children('.flowgraph-context-container')
                            .css('display', 'none');
                    });
            }
        }
    }

    #keyHandler(e) {
        if (e.code == 'Backspace' || e.code == 'Delete') {
            if (this.focusedElement) {
                if($(document.activeElement).is(':input')) {
                    return;
                }

                if ($(this.focusedElement).closest('.flowgraph-node-container').length > 0) {
                    const nodeId = this.focusedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
                    if (!this.nodes[this.nodesCache[nodeId]].fixed) {
                        this.removeNode(nodeId);
                    }
                } else if ($(this.focusedElement).closest('.flowgraph-connection').length > 0) {
                    const connectionId = this.focusedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
                    this.removeConnection(connectionId);
                }
            }
        }
    }

    #rightClickHandler(e) {
        if ($(e.target).closest('.flowgraph-context-container').length > 0) {

        } else {
            const contextWidth = 230;
            const contextHeight = 200;

            let spawnX = this.lastClientX;
            let spawnY = this.lastClientY;
            
            // Prevent overflow by X
            if(e.x + contextWidth > $(this.rootNode).width()) {
                spawnX -= contextWidth - ($(this.rootNode).width() - e.x);
            }
            
            // Prevent overflow by Y
            if(e.y + contextHeight > $(this.rootNode).height()) {
                spawnY -= contextHeight - ($(this.rootNode).height() - e.y);
            }

            $(this.rootNode)
                .children('.flowgraph-context-container')
                .css('display', 'block')
                .css('transform', `translate(${spawnX}px, ${spawnY}px)`);

            this.#renderContextMenu();
        }
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

        if ($(e.target).closest('.flowgraph-context-container').length > 0) {

        } else {
            $(this.rootNode).children('.flowgraph-context-container')
                .css('display', 'none');
        }

        if (this.focusedElement) {
            if ($(this.focusedElement).closest('.flowgraph-connection').length > 0) {
                this.focusedElement.setAttribute('stroke', 'rgb(42, 143, 42)');
                this.focusedElement.setAttribute('stroke-width', '8');
            } else {
                $(this.focusedElement).removeClass('focused');
            }
            this.focusedElement = null;
        }

        switch (this.dragMode) {
            case "io.input": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-io')
                    .get(0);

                const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
                const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                if (connection) {
                    $(this.rootNode).find(`#node_${destNode}_in_${inIndex}`)
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

                const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
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
                break;
            }
            case "connection": {
                this.selectedElement = $(e.target)
                    .closest('.flowgraph-connection')
                    .get(0);
                break;
            }
        }
    }

    #dragNodeEndHandler(e) {
        switch (this.dragMode) {
            case "io.input": {
                if ($(e.target).closest('.flowgraph-io').closest('.flowgraph-io-row.left').length > 0) {
                    let destElement = $(e.target).closest('.flowgraph-io')[0];

                    const [destNode, inIndex] = destElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
                    let connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        this.removeConnection(connection.id);
                    }

                    const [selectedNode, selectedIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
                    connection = Object.values(this.connections).find(value => (value.dest == selectedNode && value.in == selectedIndex));

                    if (connection) {
                        this.#updateConnection(connection.id, connection.source, connection.out, destNode, inIndex);
                    }
                } else {
                    const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
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

                    const [destNode, inIndex] = destElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        if (this.#nodeInputTypeCheck(sourceNode, outIndex, destNode, inIndex)) {
                            const connections = Object.values(this.connections).filter(
                                value => (value.source == this.connections[this.connectionsCache[connection.id]].source && value.out == this.connections[this.connectionsCache[connection.id]].out));
                            if (connections.length == 1) {
                                $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[connection.id]].source}_out_${this.connections[this.connectionsCache[connection.id]].out}`)
                                    .removeClass('connected')
                                    .get(0);
                            }

                            this.#updateConnection(connection.id, sourceNode, outIndex, destNode, inIndex);
                        } else {
                            $(this.rootNode).find(`#node_${sourceNode}_out_${outIndex}`)
                                .removeClass('connected')
                                .get(0);
                        }
                    } else {
                        try {
                            this.addConnection(sourceNode, outIndex, destNode, inIndex);
                        } catch (e) {
                            const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));

                            const connections = Object.values(this.connections).filter(
                                value => (value.source == sourceNode && value.out == outIndex));
                            if (connections.length == 0) {
                                $(this.rootNode).find(`#node_${sourceNode}_out_${outIndex}`)
                                    .removeClass('connected')
                                    .get(0);
                            }
                        }
                    }
                } else {
                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));

                    const connections = Object.values(this.connections).filter(
                        value => (value.source == sourceNode && value.out == outIndex));
                    if (connections.length == 0) {
                        $(this.rootNode).find(`#node_${sourceNode}_out_${outIndex}`)
                            .removeClass('connected')
                            .get(0);
                    }
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
            case "node": {
                if (this.selectedElement) {
                    this.focusedElement = this.selectedElement;
                    $(this.focusedElement).addClass('focused');

                    $(this.selectedElement).removeClass('selected');
                }
                break;
            }
            case "connection": {
                if (this.selectedElement) {
                    this.focusedElement = this.selectedElement;
                    this.focusedElement.setAttribute('stroke', 'rgb(180, 180, 180)');
                    this.focusedElement.setAttribute('stroke-width', '10');
                }
                break;
            }
        }

        this.dragMode = "none";
        this.selectedElement = null;
    }

    #nodeInputTypeCheck(sourceNode, outIndex, destNode, inIndex) {
        return this.nodes[this.nodesCache[sourceNode]].outputs[outIndex].type == this.nodes[this.nodesCache[destNode]].inputs[inIndex].type;
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

                const posX = Number(posMatrix[4]) + relativeClientX;
                const posY = Number(posMatrix[5]) + relativeClientY;

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

                    const nodeId = Number(this.selectedElement.id.match(/-?[\d]+/g)[0]);
                    const connections = Object.values(this.connections).filter(value => (value.source == nodeId || value.dest == nodeId));

                    this.nodes[this.nodesCache[nodeId]].position = [Math.ceil(posX - this.relativeCanvasX), Math.ceil(posY - this.relativeCanvasY)];

                    for (const [_, value] of Object.entries(connections)) {
                        const sourceElement = $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[value.id]].source}_out_${this.connections[this.connectionsCache[value.id]].out}`).get(0);
                        const sourceMatrix = $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[value.id]].source}`)
                            .css('transform')
                            .match(/-?[\d\.]+/g);

                        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
                        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

                        const destElement = $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[value.id]].dest}_in_${this.connections[this.connectionsCache[value.id]].in}`).get(0);
                        const destMatrix = $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[value.id]].dest}`)
                            .css('transform')
                            .match(/-?[\d\.]+/g);

                        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
                        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

                        this.#updateConnectionPosition(value.id, sourceX, sourceY, destX, destY);
                    }
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

                    const [destNode, inIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));
                    const connection = Object.values(this.connections).find(value => (value.dest == destNode && value.in == inIndex));

                    if (connection) {
                        const sourceElement = $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[connection.id]].source}_out_${this.connections[this.connectionsCache[connection.id]].out}`).get(0);
                        const sourceMatrix = $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[connection.id]].source}`)
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

                    const [sourceNode, outIndex] = this.selectedElement.id.match(/-?[\d]+/g).map((x) => (parseInt(x)));

                    const sourceElement = $(this.rootNode).find(`#node_${sourceNode}_out_${outIndex}`).get(0);
                    const sourceMatrix = $(this.rootNode).find(`#node_${sourceNode}`)
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
        if ($(e.target).closest('.flowgraph-context-container').length > 0) {

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

            const posMatrix = $(this.rootNode)
                .children('.flowgraph-canvas')
                .css('transform')
                .match(/-?[\d\.]+/g);

            const posX = Number(posMatrix[4]);
            const posY = Number(posMatrix[5]);

            $(this.rootNode)
                .children('.flowgraph-canvas')
                .css('transform', `translate(${posX}px, ${posY}px) scale(${this.zoomScale})`);

            /*$('.flowgraph-info-main')
                .empty()
                .append(`
                    <span>X: ${-this.relativeCanvasX - posX} Y: ${-this.relativeCanvasX - posY}</span>
                    <span>Zoom: ${this.zoomScale.toFixed(2)}</span>
                `);
            */
            $(this.rootNode)
                .find('.flowgraph-bg-container')
                .get(0).style.setProperty('--grid-size', `${this.zoomScale * 40}px`);
        }
    }

    removeConnection(connectionId) {
        if (this.connectionsCache[connectionId] == null) {
            throw new Error("Connection with this ID does not exist");
        }

        const connections = Object.values(this.connections).filter(
            value => (value.source == this.connections[this.connectionsCache[connectionId]].source && value.out == this.connections[this.connectionsCache[connectionId]].out));
        if (connections.length == 1) {
            $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[connectionId]].source}_out_${this.connections[this.connectionsCache[connectionId]].out}`)
                .removeClass('connected')
                .get(0);
        }

        $(this.rootNode).find(`#node_${this.connections[this.connectionsCache[connectionId]].dest}_in_${this.connections[this.connectionsCache[connectionId]].in}`)
            .removeClass('connected')
            .get(0);

        const vectorLayer = $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .get(0);

        let svgElement = vectorLayer.getElementById(`connection_${connectionId}`);
        svgElement.remove();

        this.connections.splice(this.connectionsCache[connectionId], 1);

        this.connectionsCache = {};
        for (let i = 0; i < this.connections.length; i++) {
            this.connectionsCache[this.connections[i].id] = i;
        }
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
        $(this.rootNode).find(`#node_${sourceNode}_out_${outIndex}`)
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
        const sourceElement = $(this.rootNode).find(`#node_${sourceNodeId}_out_${outIndex}`)
            .addClass('connected')
            .get(0);
        const sourceMatrix = $(this.rootNode).find(`#node_${sourceNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

        const destElement = $(this.rootNode).find(`#node_${destNodeId}_in_${inIndex}`)
            .addClass('connected')
            .get(0);
        const destMatrix = $(this.rootNode).find(`#node_${destNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

        this.connections[this.connectionsCache[connectionId]] = {
            "id": connectionId,
            "source": sourceNodeId,
            "out": outIndex,
            "dest": destNodeId,
            "in": inIndex
        };

        this.#updateConnectionPosition(connectionId, sourceX, sourceY, destX, destY);
    }

    #internalAddConnection(id, sourceNodeId, outIndex, destNodeId, inIndex) {
        if (!this.#nodeInputTypeCheck(sourceNodeId, outIndex, destNodeId, inIndex)) {
            throw new TypeError("Cannot join different data types");
        }

        const sourceElement = $(this.rootNode).find(`#node_${sourceNodeId}_out_${outIndex}`)
            .addClass('connected')
            .get(0);
        const sourceMatrix = $(this.rootNode).find(`#node_${sourceNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const sourceX = Number(sourceMatrix[4]) + sourceElement.offsetLeft + sourceElement.offsetWidth / 2;
        const sourceY = Number(sourceMatrix[5]) + sourceElement.offsetTop + sourceElement.offsetHeight / 2;

        const destElement = $(this.rootNode).find(`#node_${destNodeId}_in_${inIndex}`)
            .addClass('connected')
            .get(0);
        const destMatrix = $(this.rootNode).find(`#node_${destNodeId}`)
            .css('transform')
            .match(/-?[\d\.]+/g);

        const destX = Number(destMatrix[4]) + destElement.offsetLeft + destElement.offsetWidth / 2;
        const destY = Number(destMatrix[5]) + destElement.offsetTop + destElement.offsetHeight / 2;

        let svgElement = document.createElementNS("http://www.w3.org/2000/svg", 'path');
        svgElement.classList.add('flowgraph-connection');
        svgElement.setAttribute('id', `connection_${id}`);
        svgElement.setAttribute('stroke', 'rgb(42, 143, 42)');
        svgElement.setAttribute('stroke-width', '8');
        svgElement.setAttribute('fill', 'transparent');

        $(this.rootNode)
            .children('.flowgraph-canvas')
            .children('.flowgraph-vectorlayer')
            .append(svgElement);

        const size = this.connections.push({
            "id": id,
            "source": sourceNodeId,
            "out": outIndex,
            "dest": destNodeId,
            "in": inIndex
        });

        this.connectionsCache[id] = size - 1;

        this.#updateConnectionPosition(id, sourceX, sourceY, destX, destY);
    }

    addConnection(sourceNodeId, outIndex, destNodeId, inIndex) {
        const id = this.lastCreatedId;
        this.#internalAddConnection(id, sourceNodeId, outIndex, destNodeId, inIndex);
        this.lastCreatedId++;
        return id;
    }

    removeNode(nodeId) {
        if (this.nodesCache[nodeId] == null) {
            throw new Error("Node with this ID does not exist");
        }

        const connections = Object.values(this.connections).filter(value => (value.source == nodeId || value.dest == nodeId));
        for (const value of Object.values(connections)) {
            this.removeConnection(value.id);
        }

        $(this.rootNode).find(`#node_${nodeId}`)
            .get(0)
            .remove();

        this.nodes.splice(this.nodesCache[nodeId], 1)

        this.nodesCache = {};
        for (let i = 0; i < this.nodes.length; i++) {
            this.nodesCache[this.nodes[i].id] = i;
        }
    }

    #internalAddNode(id, x, y, inputs, outputs, nodeName, fixed, expandHTML, userData) {
        let inputsHTML = "";
        let inputMaxLength = 5;
        for (let i = 0; i < inputs.length; i++) {
            switch(inputs[i].type) {
                case 'Number': {
                    const numberValue = parseFloat(userData.options[inputs[i].name]);

                    inputsHTML += `
                        <div class="flowgraph-io-row left">
                            <div style="display: inline-flex; gap: 5px; align-items: center;">
                                <span style="color: white;">${inputs[i].name}:</span>
                                <input id="input_${id}_${i}" type="number" value="${numberValue}" placeholder="${inputs[i].type}" style="width: 80px;" />
                            </div>
                        </div>
                    `;
                    break;
                }
                case 'String': {
                    const stringValue = userData.options[inputs[i].name];

                    inputsHTML += `
                        <div class="flowgraph-io-row left">
                            <div style="display: inline-flex; gap: 5px; align-items: center;">
                                <span style="color: white;">${inputs[i].name}:</span>
                                <input id="input_${id}_${i}" type="text" value="${stringValue}" placeholder="${inputs[i].type}" style="width: 80px;" />
                            </div>
                        </div>
                    `;
                    break;
                }
                case 'Color': {
                    // https://stackoverflow.com/questions/5623838/rgb-to-hex-and-hex-to-rgb
                    const hexToRgb = (r, g, b) => {
                        return "#" + (1 << 24 | r << 16 | g << 8 | b).toString(16).slice(1);
                    };

                    const colorValue = userData.options[inputs[i].name].split(',').map(x => Math.ceil(parseFloat(x) * 255));
                    
                    inputsHTML += `
                        <div class="flowgraph-io-row left">
                            <div style="display: inline-flex; gap: 5px; align-items: center;">
                                <span style="color: white;">${inputs[i].name}:</span>
                                <input id="input_${id}_${i}" type="color" value="${hexToRgb(...colorValue)}" placeholder="${inputs[i].type}" style="width: 50px;" />
                            </div>
                        </div>
                    `;
                    break;
                }
                default: {
                    inputsHTML += `
                        <div class="flowgraph-io-row left">
                            <div id="node_${id}_in_${i}" class="flowgraph-io circle"></div>
                            <span style="color: white;">${inputs[i].name} (${inputs[i].type})</span>
                        </div>
                    `;
                    break;
                }
            }
            inputMaxLength = Math.max(inputs[i].name.length, inputMaxLength);
        }

        if (inputs.length == 0) {
            inputMaxLength = 0;
        }

        let outputsHTML = "";
        let outputMaxLength = 5;
        for (let i = 0; i < outputs.length; i++) {
            outputsHTML += `
                <div class="flowgraph-io-row right">
                    <span style="color: white;">${outputs[i].name} (${outputs[i].type})</span>
                    <div id="node_${id}_out_${i}" class="flowgraph-io circle"></div>
                </div>
            `;
            outputMaxLength = Math.max(outputs[i].name.length, outputMaxLength);
        }

        if (outputs.length == 0) {
            outputMaxLength = 0;
        }

        let iolistWidth = 100;
        if(inputs.length > 0 && outputs.length > 0) {
            iolistWidth = 50;
        }

        let bodyHTML = "";
        if (inputsHTML.length > 0) {
            bodyHTML += `
                <div class="flowgraph-io-list" style="width: ${iolistWidth}%;">
                    ${inputsHTML}
                </div>
            `;
        }
        if (outputsHTML.length > 0) {
            bodyHTML += `
                <div class="flowgraph-io-list" style="width: ${iolistWidth}%;">
                    ${outputsHTML}
                </div>
            `;
        }

        let footerHTML = "";
        if (expandHTML.length > 0) {
            footerHTML += `
                <div class="flowgraph-node-footer">
                    <div class="flowgraph-expand-container">
                        <img src="images/angle-down.svg" width="16" height="16" />
                    </div>
                </div>
                <div class="flowgraph-hidden-container" style="display: none;">
                    ${expandHTML}
                </div>
            `;
        }

        $(this.rootNode).children('.flowgraph-canvas').append(`
            <div id="node_${id}" class="flowgraph-node-container"> 
                <div class="flowgraph-node-root">
                    <div class="flowgraph-node-header">
                        <span style="color: white;">${nodeName}</span>
                    </div>

                    <div class="flowgraph-node-main">
                        ${bodyHTML}
                    </div>

                    ${footerHTML}
                </div>
            </div>
        `);

        $(this.rootNode).find(`#node_${id}`).find(":input").on('change', e => { 
            let targetName = $(e.currentTarget).parent().children('span').text();
            targetName = targetName.substring(0, targetName.length - 1);

            this.rootNode.dispatchEvent(
                new CustomEvent("flowgraph:value-changed", 
                    { 'detail': 
                        { 'nodeId': id, 
                          'targetName': targetName, 
                          'targetType': e.currentTarget.type, 
                          'value': e.currentTarget.value 
                        } 
                    }));
        });

        let nodeWidth = 110 + Math.min(1, outputs.length) * 22 * outputMaxLength + Math.min(1, inputs.length) * 22 * inputMaxLength;

        let nodeHeight = $(this.rootNode).find(`#node_${id}`).find('.flowgraph-node-header').outerHeight() +
            $(this.rootNode).find(`#node_${id}`).find('.flowgraph-node-main').outerHeight();
        if (expandHTML.length > 0) {
            nodeHeight += $(`#node_${id}`).find('.flowgraph-node-footer').outerHeight();
        }

        $(this.rootNode).find(`#node_${id}`)
            .css('width', `${nodeWidth}px`)
            .css('height', `${nodeHeight}px`);

        $(this.rootNode).find(`#node_${id}`).find('.flowgraph-expand-container').bind(
            'click',
            e => {
                let hiddenContainer = $(`#node_${id}`).find('.flowgraph-hidden-container')
                if (hiddenContainer.css('display') == 'flex') {
                    let nodeHeight = $(`#node_${id}`).find('.flowgraph-node-header').outerHeight() +
                        $(`#node_${id}`).find('.flowgraph-node-main').outerHeight() +
                        $(`#node_${id}`).find('.flowgraph-node-footer').outerHeight();

                    $(`#node_${id}`).css('height', `${nodeHeight}px`);
                    hiddenContainer.css('display', 'none');
                    $(`#node_${id}`)
                        .find('.flowgraph-expand-container')
                        .children('object')
                        .replaceWith('<object data="images/angle-down.svg" width="16" height="16" style="pointer-events: none;"></object>');
                } else {
                    let nodeHeight = $(`#node_${id}`).find('.flowgraph-node-header').outerHeight() +
                        $(`#node_${id}`).find('.flowgraph-node-main').outerHeight() +
                        $(`#node_${id}`).find('.flowgraph-node-footer').outerHeight() + 150;

                    $(`#node_${id}`).css('height', `${nodeHeight}px`);
                    hiddenContainer.css('display', 'flex');
                    $(`#node_${id}`)
                        .find('.flowgraph-expand-container')
                        .children('object')
                        .replaceWith('<object data="images/angle-up.svg" width="16" height="16" style="pointer-events: none;"></object>');
                }
            });

        $(this.rootNode).find(`#node_${id}`).css('transform', `translate(${this.relativeCanvasX + x}px, ${this.relativeCanvasY + y}px)`);

        const size = this.nodes.push({
            "id": id,
            "position": [x, y],
            "inputs": inputs,
            "outputs": outputs,
            "fixed": fixed,
            "userData": userData
        });

        this.nodesCache[id] = size - 1;
    }

    /**
     * Set title to node
     * @param {int} nodeId - ID node
     * @param {int} title - Title
     * @returns 
     */
    setTitleNode(nodeId, title) {
        if (this.nodesCache[nodeId] == null) {
            throw new Error("Node with this ID does not exist");
        }

        $(this.rootNode).find(`#node_${nodeId}`)
            .find(".flowgraph-node-header")
            .children('span')
            .html(title);
    }

    getNode(nodeId) {
        if (this.nodesCache[nodeId] == null) {
            throw new Error("Node with this ID does not exist");
        }
        return this.nodes[this.nodesCache[nodeId]];
    }

    /**
     * Add node to graph
     * @param {int} posX - X node position
     * @param {int} posY - Y node position
     * @param {Array} inputs - Input parameters
     * @param {Array} outputs - Output parameters
     * @param {string} nodeName - Name node
     * @param {bool} fixed - Ability to remove node via 'Remove Node'
     * @param {string} expandHTML - Expand body in HTML
     * @param {Object} userData - Optional user data
     * @returns ID created node
     */
    addNode(posX, posY, inputs, outputs, nodeName, fixed, expandHTML, userData = {}) {
        const createdId = this.lastCreatedId;
        this.#internalAddNode(createdId, posX, posY, inputs, outputs, nodeName, fixed, expandHTML, userData);
        this.lastCreatedId++;
        return createdId;
    }

    export() {
        return {
            "nodes": this.nodes,
            "connections": this.connections
        };
    }

    import(data) {
        for (const node of Object.values(data.nodes)) {
            this.#internalAddNode(node.id, node.position[0], node.position[1],
                node.inputs, node.outputs, node.name, node.fixed, "", node.userData);
        }

        for (const connection of Object.values(data.connections)) {
            this.#internalAddConnection(connection.id, connection.source,
                connection.out, connection.dest, connection.in);
        }

        const connectionsIds = Object.keys(this.connectionsCache).map(key => parseInt(key));
        const nodesIds = Object.keys(this.nodesCache).map(key => parseInt(key));
        this.lastCreatedId = Math.max(...connectionsIds, ...nodesIds) + 1;
    }
}