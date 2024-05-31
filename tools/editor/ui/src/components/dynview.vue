<template>
    <div class="dynview-container">
        <slot></slot>
    </div>
</template>

<script>
import $ from 'jquery'

export default {
    emits: ['resize'],
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
            .each((_, element) => {
                $(element)
                    .children()
                    .each((index, element) => {
                        if(index > 0) {
                            $(element).append(`<div class="dynsize-bar col"></div>`);
                        }
                    });
            });

        $('.dyngr-container.row')
            .each((_, element) => {
                $(element)
                    .children()
                    .each((index, element) => {
                        if(index > 0) {
                            $(element).append(`<div class="dynsize-bar row"></div>`);
                        }
                    });
            });

        $('.dynsize-bar').bind('mousedown', e => {
            this.selectedMover = e.target;
            this.isMoverVertical = $(this.selectedMover).hasClass('row') ? false : true;
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

                const updatedCurrentSize = (this.isMoverVertical ? currentPanel.height() : currentPanel.width()) + distance;
                const updatedPreviousSize = (this.isMoverVertical ? previousPanel.height() : previousPanel.width()) - distance;

                if(updatedCurrentSize < 100 || updatedPreviousSize < 100) {
                    return;
                }

                if(Math.abs(distance) > 10) {
                    return;
                }

                if(this.isMoverVertical) {
                    currentPanel.css('height', `${updatedCurrentSize}px`);
                    previousPanel.css('height', `${updatedPreviousSize}px`);
                } else {
                    currentPanel.css('width', `${updatedCurrentSize}px`);
                    previousPanel.css('width', `${updatedPreviousSize}px`);
                }
                
                this.$emit('resize', this.selectedMover);
            }
        });
    },
    methods: {
        dynViewSlotsChanged(e) {
            console.log('dynViewSlotsChanged', e)
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
    width: 100%;
    cursor: row-resize;
}

.dynsize-bar.row {
    width: 10px;
    height: 100%;
    cursor: col-resize;
}
</style>