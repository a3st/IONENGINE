// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/framework.h>

/*
    Implementation of the main function for the Windows platform
*/
int main(int* argc, char** agrv) {

    ionengine::Framework framework;

    framework.run(
        [&]() {

        }
    );

    return 0;
}