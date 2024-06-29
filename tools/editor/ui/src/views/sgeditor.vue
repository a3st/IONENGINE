<template>
    <div class="pan-wrapper" style="padding: 0; margin: 0">
        <div class="sgeditor-toolbar-container">
            <button class="btn-text" style="gap: 10px" @click="onSaveClick">
                <img
                    :src="require('../images/floppy-disk.svg')"
                    width="16"
                    height="16"
                />
                <span>Save</span>
            </button>

            <button class="btn-text" style="gap: 10px" @click="onCompileClick">
                <img
                    :src="require('../images/cubes-stacked.svg')"
                    width="16"
                    height="16"
                />
                <span>Compile</span>
            </button>
        </div>
        <graphview ref="graphView"></graphview>
    </div>
</template>

<script>
import GraphviewComponent from "../components/graph/graphview.vue";

export default {
    components: {
        graphview: GraphviewComponent,
    },
    data() {
        return {
            graphType: null,
            fileInfo: {},
        };
    },
    mounted() {
        webview.invoke("getShaderGraphComponents").then((data) => {
            for (const [groupName, groupItems] of Object.entries(data.groups)) {
                for (const item of Object.values(groupItems)) {
                    this.$refs.graphView.$refs.ctxMenu.addContextItem(
                        groupName,
                        item.name,
                        (e) => {
                            this.$refs.graphView.addNode(
                                e.x,
                                e.y,
                                item.inputs,
                                item.outputs,
                                item.name,
                                false,
                                item.userData
                            );
                        }
                    );
                }
            }
        });
    },
    methods: {
        open(graphData, fileInfo) {
            this.$refs.graphView.import(graphData.sceneData);
            this.graphType = graphData.graphType;
            this.fileInfo = fileInfo;
        },
        onSaveClick(e) {
            webview
                .invoke("shaderGraphAssetSave", this.fileInfo, {
                    graphType: this.graphType,
                    sceneData: this.$refs.graphView.export(),
                })
                .then((data) => console.log(data));
        },
        onCompileClick(e) {
            webview
                .invoke("shaderGraphAssetSave", this.fileInfo, {
                    graphType: this.graphType,
                    sceneData: this.$refs.graphView.export(),
                })
                .then((data) => console.log(data));

            webview
                .invoke("shaderGraphAssetCompile", this.fileInfo)
                .then((data) => console.log(data));
        },
    },
};
</script>

<style>
.sgeditor-toolbar-container {
    display: inline-flex;
    background-color: rgb(45, 45, 45);
    height: 30px;
    align-items: center;
    padding: 0px 10px 0px 10px;
    gap: 10px;
}
</style>