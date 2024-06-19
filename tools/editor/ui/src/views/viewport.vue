<template>
    <div class="pan-wrapper" style="padding: 0; margin: 0">
        <img
            v-bind:src="'data:image/png;base64,' + viewportImage"
            width="100%"
            height="100%"
        />
    </div>
</template>

<script>
export default {
    data() {
        return {
            viewportImage: null,
        };
    },
    methods: {
        setViewportImage(imageData) {
            this.viewportImage = imageData;
        }
    }
};
</script>