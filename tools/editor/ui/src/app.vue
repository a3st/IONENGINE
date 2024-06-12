<template>
    <router-view />
</template>

<style>
@import url('fonts/Roboto/embed.css');

* {
    box-sizing: border-box;
    font-family: 'Roboto', sans-serif;
}

img {
    user-select: none;
}

span {
    display: inline-flex;
    align-items: center;
    user-select: none;
}

body {
    margin: 0;
    background-color: rgb(65, 65, 65);
    color: white;
    overflow: hidden;
}

button {
    border: 0;
    outline: 0;
}

select {
    width: 100%; 
    padding: 5px;
    background-color: rgb(60, 60, 60); 
    color: white; 
    border: none;
}

input {
    color: white;
    background: none;
    border: none;
    outline: none;
}

::-webkit-scrollbar {
    width: 8px;
}

::-webkit-scrollbar-track {
    background: #f1f1f1;
}

::-webkit-scrollbar-thumb {
    background: #5e5e5e;
}

::-webkit-scrollbar-thumb:hover {
    background: #444444;
}
</style>