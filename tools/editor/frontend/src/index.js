import { createApp } from 'vue'
import { createRouter, createMemoryHistory } from 'vue-router'
import { createVuetify } from 'vuetify/lib/framework.mjs';
import * as components from 'vuetify/components'
import * as directives from 'vuetify/directives'
import AppView from './app.vue'
import MainView from './views/main.vue'

const vuetify = createVuetify({
    components,
    directives
});

const router = createRouter({
    routes: [
        {
            name: "Main",
            path: "/",
            component: MainView
        }
    ],
    history: createMemoryHistory()
});

createApp(AppView)
    .use(router)
    .use(vuetify)
    .mount('#app');