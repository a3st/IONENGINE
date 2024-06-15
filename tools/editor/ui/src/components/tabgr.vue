<template>
    <div class="tabgr-container">
        <slot></slot>
    </div>
</template>

<script>
import $ from 'jquery'
import TabliComponent from '../components/tabli.vue';

export default {
    props: {
        type: String
    },
    data() {
        return {
            targets: []
        }
    },
    mounted() {
        this.$slots.default().forEach((element, index) => {
            if(element.type == TabliComponent) {
                this.targets.push(element.props.target);

                $(this.$el).children().eq(index).bind('click', e => {
                    for(const target of Object.values(this.targets)) {
                        $(`#${target}`).css('display', 'none');
                    }

                    $(this.$el).find('.btn-tab').removeClass('active');
            
                    $(e.target)
                        .closest('button.btn-tab')
                        .addClass('active');

                    $(`#${element.props.target}`).css('display', 'flex');
                });

                if('default' in element.props) {
                    $(this.$el).children().eq(index)
                        .closest('button.btn-tab')
                        .addClass('active');

                    $(`#${element.props.target}`).css('display', 'flex');
                }
            }
        });
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