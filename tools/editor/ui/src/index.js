import { createApp } from 'vue'
import { createRouter, createMemoryHistory } from 'vue-router'
import AppView from './app.vue'
import MainView from './views/main.vue'

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
    .mount('#app');