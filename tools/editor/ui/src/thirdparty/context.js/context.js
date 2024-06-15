// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

import $ from 'jquery'

export default class ContextMenu {
    constructor(target, contextName, contextItems) {
        this.target = target;
        this.items = [];
        this.groups = [];
        this.shortcuts = {};

        let itemsHTML = "";

        for(const group of Object.values(contextItems)) {
            const groupsSize = this.groups.push([]);

            itemsHTML += `<div class="context-group-container">`;

            for(const item of Object.values(group.items)) {
                const itemsSize = this.items.push({
                    'name': item.name,
                    'action': item.action,
                    'shortcut': item.shortcut
                });

                this.groups[groupsSize - 1].push(itemsSize - 1);

                // TODO! Shortcuts

                itemsHTML += `
                    <button>
                        <span>${item.name}</span>
                        <span>${item.shortcut}</span>
                    </button>
                `;
            }
        }

        $(document.body).append(`
            <div id="${contextName}" class="context-container">
                <div class="context-container-body">
                    ${itemsHTML}
                </div>
            </div>
        `);
        $(`#${contextName}`).css('display', 'none');

        this.target.addEventListener('contextmenu', e => {
            e.preventDefault();

            $(`#${contextName}`)
            .css('left', e.pageX)
            .css('top', e.pageY)
            .css('display', 'block');
        });

        document.addEventListener('mousedown', e => {
            if ($(e.target).closest('.context-container').length > 0) {

            } else {
                $(`#${contextName}`).css('display', 'none');
            }
        });
    }
}