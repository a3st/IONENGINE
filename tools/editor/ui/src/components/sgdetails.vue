<template>
    <div v-if="type == 'resources'" class="pan-wrapper" style="gap: 10px;">
        <optext v-for="(resource, index) in resources" icon="images/buffer.svg" @remove="onResourceDeleteClick($event, index)">
            <template #header>
                <input type="text" placeholder="Type name" v-model="resource.name">
            </template>
            <template #expand>
                <optli title="Type" type="select" v-model="resource.type" v-bind:content="resourceTypes"></optli>
            </template>
        </optext>

        <button class="btn" @click="onResourceAddClick($event)">Add New Resource</button>
    </div>
    
    <div v-else-if="type == 'shader'" class="pan-wrapper" style="gap: 10px;">
        <optgr v-if="domain == 'unlit'">
            <optli title="Surface" type="select" v-model="shaderDomain" v-bind:content="shaderDomains"></optli>
            <optli title="Blend" type="select"></optli>
            <optli title="Double Sided" type="checkbox"></optli>
        </optgr>
        <optgr v-else="domain == 'lit'">

        </optgr>
    </div>
</template>

<script>
import OptextComponent from '../components/optext.vue';
import OptliComponent from '../components/optli.vue';
import OptgrComponent from '../components/optgr.vue';

export default {
    components: {
        'optext': OptextComponent,
        'optli': OptliComponent,
        'optgr': OptgrComponent,
    },
    props: {
        type: String
    },
    data() {
        return {
            resourceTypes: [
                "Texture2D", "float4", "float3", "float2", "float", "bool"
            ],
            resources: [],
            domain: null
        }
    },
    mounted() {

    },
    methods: {
        onResourceAddClick(e) {
            this.resources.push({
                'name': 'newResource',
                'type': 0
            });
        },
        onResourceDeleteClick(e, index) {
            this.resources.splice(index, 1);
        }
    }
}

/*resources: {
            handler(value, oldValue) {
                let posX = 0;
                let posY = 0;
                if(this.resourceNodeId != -1) {
                    [posX, posY] = toRaw(this.graph).getNode(this.resourceNodeId).position;
                    toRaw(this.graph).removeNode(this.resourceNodeId);
                    this.resourceNodeId = -1;
                }

                let outputsData = [];
                for(const resource of Object.values(value)) {
                    outputsData.push({
                        "name": resource.name,
                        "type": this.resourceTypes[resource.type]
                    });
                }
                
                if(outputsData.length > 0) {
                    this.resourceNodeId = toRaw(this.graph).addNode(
                        posX, posY, 
                        this.inputNode.inputs, 
                        outputsData, 
                        this.inputNode.name, 
                        this.inputNode.fixed, 
                        "", 
                        this.inputNode.userData);
                }
            },
            deep: true
        },
        shaderDomain: {
            handler(value, oldValue) {
                let posX = 0;
                let posY = 0;
                if(this.shaderDomainNodeId != -1) {
                    [posX, posY] = toRaw(this.graph).getNode(this.shaderDomainNodeId).position;
                    toRaw(this.graph).removeNode(this.shaderDomainNodeId);
                    this.shaderDomainNodeId = -1;
                }
                
                const node = this.domainNodes[value];
                this.shaderDomainNodeId = toRaw(this.graph).addNode(
                    posX, posY, 
                    node.inputs, 
                    node.outputs, 
                    node.name, 
                    node.fixed, 
                    "", 
                    node.userData);
            }
        },*/
</script>