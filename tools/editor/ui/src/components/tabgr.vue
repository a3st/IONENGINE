<template>
    <div class="tabgr-container">
        <slot></slot>
    </div>
</template>

<script>
import { toRaw } from 'vue';
import $ from 'jquery'
import TabliComponent from '../components/tabli.vue';

export default {
    emits: ['remove', 'select'],
    props: {
        type: String
    },
    data() {
        return {
            targets: []
        }
    },
    updated() {
        this.updateTabSlots();
    },
    mounted() {
        this.updateTabSlots();
    },
    methods: {
        updateTabSlots() {
            this.targets.splice(0, this.targets.length);

            $(this.$el).children().each((_, element) => {
                if(element.__vueParentComponent.proxy.$.type == TabliComponent) {
                    element.__vueParentComponent.proxy.$parent.$data.targets.push(element.__vueParentComponent.proxy.$props.target);

                    if(element.__vueParentComponent.proxy.$props.default) {
                        $(element)
                            .closest('button.btn-tab')
                            .addClass('active');

                        $(`#${element.__vueParentComponent.proxy.$props.target}`).css('display', 'flex');
                    }
                }
            });
        },
        setActiveTabByIndex(index) {
            for(const target of Object.values(this.targets)) {
                $(`#${target}`).css('display', 'none');
            }

            $(this.$el).find('.btn-tab').removeClass('active');

            const target = $(this.$el).children().eq(index);
            target
                .closest('button.btn-tab')
                .addClass('active');

            this.$emit('select', target.get(0));

            $(`#${target.get(0).__vueParentComponent.proxy.$props.target}`).css('display', 'flex');
        }
    }
}
</script>

<style>
.tabgr-container {
    display: flex; 
    flex-direction: row; 
    height: 30px; 
    background-color: rgb(50, 50, 50); 
    width: 100%;
}
</style>