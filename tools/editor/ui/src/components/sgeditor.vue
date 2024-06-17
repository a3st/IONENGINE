<template>
    <div class="pan-wrapper" style="padding: 0; margin: 0;">
        <div style="background-color: rgb(45, 45, 45); height: 30px; display: flex;">
            <div style="display: inline-flex; align-items: center; gap: 10px; height: 100%; padding: 0px 10px 0px 10px;">
                <button class="btn-text" @click="onCompileShaderClick($event)">Compile</button>
            </div>
        </div>
        <div ref="graph"></div>
    </div>
</template>

<script>
import { toRaw } from 'vue';
import $ from 'jquery';
import FlowGraph from '../thirdparty/flowgraph.js/flowgraph';

export default {
    data() {
        return {
            graph: null
        }
    },
    created() {
        window.addEventListener("resize", this.onGraphResize);
    },
    destroyed() {
        window.removeEventListener("resize", this.onGraphResize);
    },
    updated() {
        const graphElement = $(this.$refs.graph);
        toRaw(this.graph).resize(graphElement.parent().width(), graphElement.parent().height() - 30);
    },
    mounted() {
        const graphElement = $(this.$refs.graph);
        const graph = new FlowGraph(graphElement.get(0), graphElement.parent().width(), graphElement.parent().height() - 30);

        graph.start({
            'valueChanged': e => {
                const node = toRaw(this.graph).getNode(e.detail.nodeId);
                switch(e.detail.targetType) {
                    case 'color': {
                        // https://stackoverflow.com/questions/5623838/rgb-to-hex-and-hex-to-rgb
                        const hexToRgb = hex =>
                            hex.replace(/^#?([a-f\d])([a-f\d])([a-f\d])$/i
                                        ,(m, r, g, b) => '#' + r + r + g + g + b + b)
                                .substring(1).match(/.{2}/g)
                                .map(x => parseInt(x, 16));

                        const userData = {
                            'componentID': node.userData.componentID,
                            'options': {...node.userData.options}
                        };

                        userData.options[e.detail.targetName] = hexToRgb(e.detail.value).map(x => Number(x / 255).toFixed(3)).join();
                        node.userData = userData;
                        break;
                    }
                    default: {
                        const userData = {
                            'componentID': node.userData.componentID,
                            'options': {...node.userData.options}
                        };

                        userData.options[e.detail.targetName] = e.detail.value;
                        node.userData = userData;
                        break;
                    }
                }
            }
        });

        this.graph = graph;
    },
    methods: {
        onGraphResize(e) {
            const graphElement = $(this.$refs.graph);
            toRaw(this.graph).resize(graphElement.parent().width(), graphElement.parent().height() - 30);
        },
        open(graphData) {
            toRaw(this.graph).import(graphData);
        }
    }
}
</script>