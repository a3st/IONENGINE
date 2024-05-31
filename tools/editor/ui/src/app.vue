<template>
    <router-view />
</template>

<style>
* {
    box-sizing: border-box;
}

span {
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
</style>