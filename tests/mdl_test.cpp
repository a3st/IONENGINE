// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "mdl/obj/obj.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(MDL, LoadOBJ_Test)
{
    auto objImporter = core::make_ref<asset::OBJImporter>();
    auto modelFile = objImporter->loadFromFile("../../assets/objects/box.obj");
    ASSERT_TRUE(modelFile.has_value());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}