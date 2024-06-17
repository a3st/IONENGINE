<template>
    <div class="tabgr-container">
        <slot></slot>
    </div>
</template>

<script>
import $ from 'jquery'
import TabliComponent from '../components/tabli.vue';

export default {
    emits: ['remove'],
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
            $(this.$el).children().each((index, element) => {
                if(element.__vueParentComponent.type == TabliComponent) {
                    this.targets.push(element.__vueParentComponent.props.target);

                    element.addEventListener('click', e => {
                        for(const target of Object.values(this.targets)) {
                            $(`#${target}`).css('display', 'none');
                        }

                        $(this.$el).find('.btn-tab').removeClass('active');

                        $(e.target)
                            .closest('button.btn-tab')
                            .addClass('active');

                        $(`#${element.__vueParentComponent.props.target}`).css('display', 'flex');
                    });

                    element.__vueParentComponent.emitsOptions.remove = e => {
                        this.$emit('remove', element);
                    };

                    if(element.__vueParentComponent.props.default) {
                        $(element)
                            .closest('button.btn-tab')
                            .addClass('active');

                        $(`#${element.__vueParentComponent.props.target}`).css('display', 'flex');
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

            $(`#${target.get(0).__vueParentComponent.props.target}`).css('display', 'flex');
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