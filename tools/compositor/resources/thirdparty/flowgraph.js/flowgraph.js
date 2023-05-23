import 'https://code.jquery.com/jquery-3.6.0.min.js';
const $ = window.$;

export default class FlowGraph {
    constructor(rootNode) {
        this.rootNode = rootNode;
        this.zoomScale = 1.0;
        this.selectedNode = null;
    }

    start() {
        $(this.rootNode).addClass('flowgraph-container');
        $(this.rootNode).append(`
            <div class="flowgraph-bg-container"></div>
        `);

        this.rootNode.addEventListener('mousewheel', this.zoom_event_handler.bind(this));
        this.rootNode.addEventListener('mousedown', this.drag_node_begin_handler.bind(this));
        this.rootNode.addEventListener('mouseup', this.drag_node_end_handler.bind(this));
        this.rootNode.addEventListener('mousemove', this.move_node_handler.bind(this));
    }

    drag_node_begin_handler(e) {
        if($(e.target).closest('.flowgraph-io').length > 0) {
            return
        }

        this.selectedNode = $(e.target)
            .closest('.flowgraph-node-container')
            .get(0);
    }

    drag_node_end_handler(e) {
        if(this.selectedNode) {
            this.selectedNode = null;
        }
    }

    move_node_handler(e) {
        if(this.selectedNode) {
            const posX = e.clientX - this.selectedNode.offsetWidth / 2;
            const posY = e.clientY - this.selectedNode.offsetHeight / 2;

            $(this.selectedNode).css('transform', `translate(${posX}px, ${posY}px)`)
        }
    }

    zoom_event_handler(e) {
        if(e.deltaY > 0) {
            this.zoomScale += 0.1;
        } else {
            this.zoomScale -= 0.1;
        }
        // console.log(this.zoomScale)
    }

    add_node(x, y, inputs, outputs, headerHTML) {
        $(this.rootNode).append(`
            <div class="flowgraph-node-container" style="width: 350px; height: 200px;"> 
                <div class="flowgraph-node-root">
                    <div class="flowgraph-node-header">
                        ${headerHTML}
                    </div>

                    <div class="flowgraph-node-main">
                        <div class="flowgraph-io-list">
                            <div class="flowgraph-io-row left">
                                <div class="flowgraph-io circle"> </div>
                                <span style="color: white;">Albedo (float3)</span>
                            </div>

                            <div class="flowgraph-io-row left">
                                <div class="flowgraph-io circle"> </div>
                                <span style="color: white;">Normal (float3)</span>
                            </div>

                            <div class="flowgraph-io-row left">
                                <div class="flowgraph-io circle"> </div>
                                <span style="color: white;">Roughness (float3)</span>
                            </div>

                            <div class="flowgraph-io-row left">
                                <div class="flowgraph-io circle"> </div>
                                <span style="color: white;">Metalness (float3)</span>
                            </div>
                        </div>

                        <div class="flowgraph-io-list">
                            <div class="flowgraph-io-row right">
                                <span style="color: white;">Color (float3)</span>
                                <div class="flowgraph-io circle"> </div>
                            </div>
                        </div>
                    <div>
                </div>
            </div>
        `)
    }
}