<template>
    <div class="pan-wrapper" style="padding: 0; margin: 0;">
        <img v-bind:src="'data:image/png;base64,' + previewImageSource" width="100%" height="100%" />
    </div>
</template>

<script>
export default {
    data() {
        return {
            previewImageSource: null
        }
    }
}

/*
onFrameUpdate() {
            webview.invoke('requestPreviewImage').then(data => {
                this.previewImageSource = data;
                window.requestAnimationFrame(this.onFrameUpdate);
            });
        },
*/
</script>