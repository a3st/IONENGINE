// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <tclap/CmdLine.h>
#include <lib/algorithm.h>

uint32_t const MAJOR_VERSION = 1;
uint32_t const MINOR_VERSION = 0;

int main(int argc, char** argv) {

    std::cout << std::format("Welcome to Asset Compiler Tool [Version {}.{}]", MAJOR_VERSION, MINOR_VERSION) << std::endl;
    std::cout << "(c) 2022 Dmitriy Lukovenko. All rights reserved" << std::endl;

    try {

        TCLAP::CmdLine cmd_line("", ' ', std::format("{}.{}", MAJOR_VERSION, MINOR_VERSION));

        

    } catch(TCLAP::ArgException& exception) {
        std::cerr << std::format("[Error] {} for argument {}", exception.error(), exception.argId()) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[Info] Compilation of assets completed successfully" << std::endl;
    return EXIT_SUCCESS;
}