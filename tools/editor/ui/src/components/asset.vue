<template>
    <div
        class="asset-container"
        @mousedown="onAssetClick($event)"
        @dblclick="onAssetDblClick($event)"
    >
        <img
            style="object-fit: contain"
            v-bind:src="
                type == 'folder'
                    ? require('../images/folder.svg')
                    : require('../images/file.svg')
            "
            width="64"
            height="64"
        />
        <input
            ref="editNameInput"
            v-if="edit"
            style="width: 64px; height: 15px"
            :value="curName"
            @blur="onEditNameBlur($event)"
            @keydown="onEditKeydown($event)"
        />
        <span
            v-else
            style="align-self: center; height: 15px"
            :title="curName"
            >{{ smallName }}</span
        >
    </div>
</template>

<script>
export default {
    emits: ["dblclick", "click", "rename"],
    props: {
        name: String,
        type: String,
    },
    data() {
        return {
            edit: false,
            curName: this.name,
        };
    },
    watch: {
        edit: {
            handler(value, oldValue) {
                if (value) {
                    this.$nextTick(() => {
                        this.$refs.editNameInput.select();
                    });
                }
            },
        },
        name: {
            handler(value, oldValue) {
                this.curName = this.lastName = value;
            },
        },
    },
    computed: {
        smallName() {
            return this.curName.length < 8
                ? this.curName
                : this.curName.substring(0, 5) + "...";
        },
    },
    methods: {
        onAssetClick(e) {
            this.$emit("click", e);
        },
        onAssetDblClick(e) {
            this.$emit("dblclick", e);
        },
        onEditNameBlur(e) {
            if (this.edit && this.curName.length != 0) {
                this.$emit("rename", e.target.value);
            }
            this.edit = false;
        },
        editName() {
            this.edit = true;
            this.lastName = this.curName;
            this.$nextTick(() => {
                this.$refs.editNameInput.select();
            });
        },
        onEditKeydown(e) {
            if (e.code == "Enter") {
                if (e.target.value.length != 0) {
                    e.target.blur();
                }
                this.edit = false;
            } else if (e.code == "Escape") {
                this.edit = false;
            }
        },
    },
};
</script>

<style>
.asset-container {
    display: flex;
    flex-direction: column;
    color: white;
    padding: 10px 15px 10px 15px;
    background-color: #404040;
}

.asset-container:hover {
    background-color: #335a7c;
}

.asset-container > span {
    font-size: 14px;
}

.asset-container > input {
    font-size: 14px;
    text-align: center;
}
</style>