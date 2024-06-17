<template>
    <div v-if="type == 'resources'" class="pan-wrapper" style="gap: 10px;">
        <optext v-for="(resource, index) in resources" icon="images/buffer.svg" @remove="onDeleteResourceClick($event, index)">
            <template #header>
                <input type="text" placeholder="Type name" v-model="resource.name">
            </template>
            <template #expand>
                <optli title="Type" type="select" v-model="resource.type" v-bind:content="resourceTypes"></optli>
            </template>
        </optext>

        <button class="btn" @click="onAddResourceClick($event)">Add New Resource</button>
    </div>
    
    <div v-else-if="type == 'shader'" class="pan-wrapper" style="gap: 10px;">
        <optgr>
            <optli title="Shader Domain" type="select" v-model="shaderDomain" v-bind:content="shaderDomains"></optli>
        </optgr>
    </div>
</template>

<script>
export default {
    props: {
        type: String
    }
}
</script>