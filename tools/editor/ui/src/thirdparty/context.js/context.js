// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

import $ from 'jquery'

class ContextMenu {
    constructor() {
        this.items = [];
        this.groups = [];
        this.shortcuts = {};

        $(document.body).append(`
            <div class="context-container">
                <div class="context-container-body"></div>
            </div>
        `);
        $('.context-container').css('display', 'none');

        $('.context-container').get(0).addEventListener('contextmenu', e => {
            e.preventDefault();
        });

        document.addEventListener('mousedown', e => {
            if ($(e.target).closest('.context-container').length > 0) {

            } else {
                $('.context-container').css('display', 'none');
            }
        });
    }

    show(event, contextItems) {
        this.items.splice(0, this.items.length);
        this.groups.splice(0, this.groups.length);
        this.shortcuts = {};

        let contextHeight = 0;

        const contextMenuElement = $('.context-container')
            .children('.context-container-body')
            .empty();

        for(const group of Object.values(contextItems)) {
            const groupsSize = this.groups.push([]);

            contextMenuElement.append(`<div class="context-group-container"></div>`);

            const groupMenuElement = $('.context-group-container').last();

            for(const item of Object.values(group.items)) {
                const itemsSize = this.items.push({
                    'name': item.name,
                    'action': item.action,
                    'shortcut': item.shortcut
                });

                this.groups[groupsSize - 1].push(itemsSize - 1);

                // TODO! Shortcuts

                let shortcutName;
                if(item.shortcut) {
                    shortcutName = item.shortcut;
                } else {
                    shortcutName = "";
                }

                groupMenuElement.append(`
                    <button>
                        <span style="width: 80%;">${item.name}</span>
                        <span style="width: 20%;">${shortcutName}</span>
                    </button>
                `);

                $('.context-group-container button')
                    .last()
                    .bind('click', e => {
                        item.action();
                        $('.context-container')
                            .css('display', 'none');
                    });

                contextHeight += 26;
            }

            contextHeight += 12;
        }

        $('.context-container')
            .css('height', contextHeight);
        
        event.target.addEventListener('contextmenu', e => {
            e.preventDefault();
        });

        $('.context-container')
            .css('left', event.pageX)
            .css('top', event.pageY)
            .css('display', 'block');
    }
}

const contextMenu = new ContextMenu();
export default contextMenu;