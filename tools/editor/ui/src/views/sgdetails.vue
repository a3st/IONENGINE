<template>
    <div
        v-if="type == 'resources'"
        class="pan-wrapper"
        style="background-color: #2f2f2f; gap: 10px"
    >
        <optext
            v-for="(resource, index) in resources"
            :key="index"
            :icon="require('../images/buffer.svg')"
            @remove="onResourceDeleteClick($event, index)"
        >
            <template #header>
                <input
                    type="text"
                    placeholder="Type name"
                    v-model="resource.name"
                />
            </template>
            <template #expand>
                <optli
                    title="Type"
                    type="select"
                    v-model="resource.type"
                    v-bind:content="resourceTypes"
                ></optli>
            </template>
        </optext>

        <button class="btn" @click="onResourceAddClick($event)">
            Add New Resource
        </button>
    </div>

    <div
        v-else-if="type == 'shader'"
        class="pan-wrapper"
        style="background-color: #2f2f2f; gap: 10px"
    >
        <optgr v-if="graphType == 'shader/unlit'">
            <optli
                title="Surface"
                type="select"
                v-model="surface"
                v-bind:content="surfaceTypes"
            ></optli>
            <optli
                title="Blend"
                type="select"
                v-model="blend"
                v-bind:content="blendTypes"
            ></optli>
            <optli
                title="Double Sided"
                type="checkbox"
                v-model="doubleSide"
            ></optli>
        </optgr>
        <optgr v-else-if="graphType == 0"></optgr>
    </div>
</template>

<script>
import { toRaw } from "vue";

import OptextComponent from "../components/optext.vue";
import OptliComponent from "../components/optli.vue";
import OptgrComponent from "../components/optgr.vue";

export default {
    components: {
        optext: OptextComponent,
        optli: OptliComponent,
        optgr: OptgrComponent,
    },
    props: {
        type: String,
    },
    watch: {
        resources: {
            handler(value, oldValue) {
                let outputsData = [];
                for (const resource of Object.values(value)) {
                    outputsData.push({
                        name: resource.name,
                        type: this.resourceTypes[resource.type],
                    });
                }

                const inputNode = this.graph.getNode(this.cacheInputNodeId);

                this.graph.updateNode(
                    inputNode.id,
                    inputNode.position[0],
                    inputNode.position[1],
                    inputNode.inputs,
                    outputsData,
                    inputNode.name,
                    inputNode.fixed,
                    inputNode.userData
                );
            },
            deep: true,
        },
        surface: {
            handler(value, oldValue) {
                const outputNode = this.graph.getNode(this.cacheOutputNodeId);

                const inputColorIndex = outputNode.inputs.findIndex(
                    (element) => element.name == "Color"
                );

                const surfaceToTypes = {
                    0: "float3",
                    1: "float4",
                };

                if (
                    outputNode.inputs[inputColorIndex].type !=
                    surfaceToTypes[value]
                ) {
                    outputNode.inputs[inputColorIndex].type =
                        surfaceToTypes[value];

                    this.cacheOutputNodeId = this.graph.updateNode(
                        outputNode.id,
                        outputNode.position[0],
                        outputNode.position[1],
                        outputNode.inputs,
                        outputNode.outputs,
                        outputNode.name,
                        outputNode.fixed,
                        outputNode.userData
                    );
                }
            },
        },
    },
    data() {
        return {
            resourceTypes: [
                "Texture2D",
                "float4",
                "float3",
                "float2",
                "float",
                "bool",
            ],
            surfaceTypes: ["Opaque", "Transparent"],
            blendTypes: ["Opaque", "Add", "Mixed", "Alpha Blend"],
            surface: 0,
            blend: 0,
            doubleSide: false,
            resources: [],
            graphType: null,
            graph: null,
            cacheInputNodeId: null,
            cacheOutputNodeId: null,
        };
    },
    methods: {
        onResourceAddClick(e) {
            this.resources.push({
                name: "newResource",
                type: 0,
            });
        },
        onResourceDeleteClick(e, index) {
            this.resources.splice(index, 1);
        },
        refresh(graphComponent, graphType) {
            this.graph = graphComponent;
            this.graphType = graphType;

            this.cacheInputNodeId =
                this.graph.getNodesByName("Input Node")[0].id;

            this.cacheOutputNodeId =
                this.graph.getNodesByName("Output Node")[0].id;

            const outputNode = this.graph.getNode(this.cacheOutputNodeId);

            const inputColorIndex = outputNode.inputs.findIndex(
                (element) => element.name == "Color"
            );

            const typesToSurface = {
                float3: 0,
                float4: 1,
            };
            this.surface =
                typesToSurface[outputNode.inputs[inputColorIndex].type];
        },
    },
};
</script>