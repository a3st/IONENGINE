import { createApp } from 'vue'
import { createRouter, createMemoryHistory } from 'vue-router'
import AppView from './app.vue'
import MainView from './views/main.vue'
import HelloView from './views/hello.vue'

const router = createRouter({
    routes: [
        {
            name: "Hello",
            path: "/",
            component: HelloView
        },
        {
            name: "Main",
            path: "/main",
            component: MainView
        }
    ],
    history: createMemoryHistory()
});

createApp(AppView)
    .use(router)
    .mount('#app');