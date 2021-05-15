// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "lib/fmt.h"
#include "lib/math.h"

using namespace ionengine;

int32 main(int32, char**) {
    
    std::cout << "Hello, world!\n";
    std::cout << ionengine::format<std::string>("123 {}", 1.0) << std::endl;
    std::wcout << ionengine::format<std::wstring>(L"123 {}", 1.0) << std::endl;
}
