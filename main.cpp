// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "lib/fmt.h"

int main(int, char**) {
    std::cout << "Hello, world!\n";
    std::cout << ionengine::format<std::string>("123 {}", 1.0) << std::endl;
}
