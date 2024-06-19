<template>
    <div class="dynview-container">
        <slot></slot>
    </div>
</template>

<script>
import $ from "jquery";

export default {
    emits: ["resize"],
    data() {
        return {
            selectedMover: null,
            moverPos: 0,
            isMoverVertical: false,
        };
    },
    mounted() {
        this.createResizeBars();

        document.addEventListener("mouseup", (e) => {
            if (this.selectedMover) {
                this.selectedMover = null;
            }
        });

        document.addEventListener("mousemove", (e) => {
            if (this.selectedMover) {
                const distance =
                    this.moverPos -
                    (this.isMoverVertical ? e.offsetY : e.offsetX);
                const currentPanel = $(this.selectedMover).parent();
                const previousPanel = $(this.selectedMover).parent().prev();

                const updatedCurrentSize =
                    (this.isMoverVertical
                        ? currentPanel.height()
                        : currentPanel.width()) + distance;
                const updatedPreviousSize =
                    (this.isMoverVertical
                        ? previousPanel.height()
                        : previousPanel.width()) - distance;

                if (updatedCurrentSize < 150 || updatedPreviousSize < 150) {
                    return;
                }

                if (Math.abs(distance) > 30) {
                    return;
                }

                if (this.isMoverVertical) {
                    $(currentPanel)
                        .find(".dyngr-container.col")
                        .find(".dynpan-container")
                        .each(function (index) {
                            const height = $(this).height();
                            const parentHeight = $(this).parent().height();

                            $(this).css(
                                "height",
                                `${((100 * height) / parentHeight).toFixed(2)}%`
                            );
                        });

                    $(previousPanel)
                        .find(".dyngr-container.col")
                        .find(".dynpan-container")
                        .each(function (index) {
                            const height = $(this).height();
                            const parentHeight = $(this).parent().height();

                            $(this).css(
                                "height",
                                `${((100 * height) / parentHeight).toFixed(2)}%`
                            );
                        });
                } else {
                    $(currentPanel)
                        .find(".dyngr-container.row")
                        .find(".dynpan-container")
                        .each(function (index) {
                            const width = $(this).width();
                            const parentWidth = $(this).parent().width();

                            $(this).css(
                                "width",
                                `${((100 * width) / parentWidth).toFixed(2)}%`
                            );
                        });

                    $(previousPanel)
                        .find(".dyngr-container.row")
                        .find(".dynpan-container")
                        .each(function (index) {
                            const width = $(this).width();
                            const parentWidth = $(this).parent().width();

                            $(this).css(
                                "width",
                                `${((100 * width) / parentWidth).toFixed(2)}%`
                            );
                        });
                }

                if (this.isMoverVertical) {
                    currentPanel.css("height", `${updatedCurrentSize}px`);
                    previousPanel.css("height", `${updatedPreviousSize}px`);
                } else {
                    currentPanel.css("width", `${updatedCurrentSize}px`);
                    previousPanel.css("width", `${updatedPreviousSize}px`);
                }

                this.$emit("resize", this.selectedMover);
            }
        });
    },
    methods: {
        onPanelCreate(e) {
            this.createResizeBars();
        },
        createResizeBars() {
            $(".dynsize-bar").remove();

            $(".dyngr-container.col").each((_, element) => {
                $(element)
                    .children()
                    .each((index, element) => {
                        if (index > 0) {
                            $(element).append(
                                `<div class="dynsize-bar col"></div>`
                            );
                        }
                    });
            });

            $(".dyngr-container.row").each((_, element) => {
                $(element)
                    .children()
                    .each((index, element) => {
                        if (index > 0) {
                            $(element).append(
                                `<div class="dynsize-bar row"></div>`
                            );
                        }
                    });
            });

            $(".dynsize-bar").on("mousedown", (e) => {
                if (this.selectedMover == null) {
                    this.selectedMover = e.target;
                    this.isMoverVertical = $(this.selectedMover).hasClass("row")
                        ? false
                        : true;
                    this.moverPos = this.isMoverVertical
                        ? e.offsetY
                        : e.offsetX;
                }
            });
        },
    },
};
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