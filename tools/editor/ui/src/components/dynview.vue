<template>
    <div class="dynview-container">
        <slot></slot>
    </div>
</template>

<script>
import $ from 'jquery'

export default {
    data() {
        return {
            observer: null,
            selectedMover: null,
            moverPos: 0,
            isMoverVertical: false
        };
    },
    mounted() {
        const observer = new MutationObserver(this.dynViewSlotsChanged);
        observer.observe(this.$el, {
            childList: true,
            subtree: false
        });
        this.observer = observer;

        $('.dyngr-container.col')
            .children()
            .each(function (index, element) {
                if(index > 0) {
                    $(element).append(`<div class="dynsize-bar col" style="width: ${$(element).width()}px;"></div>`);
                }
            });

        $('.dynsize-bar').bind('mousedown', e => {
            this.selectedMover = e.target;
            this.isMoverVertical = true;
            this.moverPos = this.isMoverVertical ? e.offsetY : e.offsetX;
        });

        $(window).bind('mouseup', e => {
            if(this.selectedMover) {
                this.selectedMover = null;
            }
        });

        $(window).bind('mousemove', e => {
            if(this.selectedMover) {
                const distance = this.moverPos - (this.isMoverVertical ? e.offsetY : e.offsetX);
                const currentPanel = $(this.selectedMover).parent();
                const previousPanel = $(this.selectedMover).parent().prev();

                const updatedCurrentSize = currentPanel.height() + distance;
                const updatedPreviousSize = previousPanel.height() - distance;

                if(updatedCurrentSize < 100 || updatedPreviousSize < 100) {
                    return;
                }

                if(Math.abs(distance) > 10) {
                    return;
                }

                currentPanel.css('height', `${updatedCurrentSize}px`);
                previousPanel.css('height', `${updatedPreviousSize}px`);
            }
        });
    },
    methods: {
        dynViewSlotsChanged(e) {
            console.log(e)
        }
    }
}
</script>

<style>
.dynview-container {
    display: flex;
    flex-direction: row;
    height: calc(100% - 60px);
}

.dynsize-bar {
    display: flex;
    position: absolute;
    background-color: transparent;
    user-select: none;
}

.dynsize-bar:hover {
    background-color: rgb(160, 160, 160);
}

.dynsize-bar.col {
    height: 10px;
    cursor: row-resize;
}

.dynsize-bar.row {
    width: 10px;
    cursor: col-resize;
}
</style>