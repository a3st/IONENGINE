// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "importer.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(MDL, LoadOBJ_Test)
{
    auto mdlImporter = asset::MDLImporter::create();
    auto modelFile = mdlImporter->loadFromFile("objects/box.obj");
    ASSERT_TRUE(modelFile.has_value());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}