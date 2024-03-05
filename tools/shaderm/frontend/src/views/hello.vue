<template>
    <div style="display: block; position: absolute; margin: auto; top: 0; bottom: 0; left: 0; right: 0; width: 500px; height: 200px;">
        <div style="display: flex; flex-direction: column; gap: 20px; align-items: center;">
            <h1>С возвращением!</h1>
            <p>Откройте проект или создайте новый</p>
            <div style="display: flex; flex-direction: row; gap: 20px;">
                <button @click="openProject">Открыть</button>
                <button @click="newProject">Новый</button>
            </div>
        </div>
    </div>
</template>

<script>
export default {
    methods: {
        openProject() {
            this.$router.push({ path: '/main' });
        },
        newProject() {
            webview.invoke('test');
        }
    }
}
</script>

<style>
h1, p {
    margin: 0;
}

p {
    font-size: 24px;
}

body {
    background-color: rgb(65, 65, 65);
    color: white;
}

button {
    border: 0;
    outline: 0;
    background-color: rgb(247, 247, 247);
    padding: 10px 15px 10px 15px;
    border-radius: 4px;
    font-size: 16px;
}

button:hover {
    background-color: aliceblue;
}
</style>