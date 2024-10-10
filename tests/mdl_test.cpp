// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "mdl/obj/obj.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(MDL, LoadOBJ_Test)
{
    auto objImporter = core::make_ref<mdl::OBJImporter>();

    std::string errors;
    auto modelFile = objImporter->loadFromFile("box.obj", errors);
    std::cout << errors << std::endl;
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}