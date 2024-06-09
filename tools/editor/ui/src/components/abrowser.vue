<template>
    <dyngr type="row">
        <dynpan>
            <div class="pan-wrapper" style="background-color: #2f2f2f;">
                <div style="flex-direction: column;">
                    <assetli></assetli>
                </div>
            </div>
        </dynpan>
        <dynpan>

        </dynpan>
    </dyngr>
</template>

<script>
import DynpanComponent from '../components/dynpan.vue';
import DyngrComponent from '../components/dyngr.vue';
import AssetliComponent from '../components/assetli.vue'

export default {
    components: {
        'dynpan': DynpanComponent,
        'dyngr': DyngrComponent,
        'assetli': AssetliComponent
    }
}
</script>

<style>

</style>