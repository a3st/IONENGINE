// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "txe/cmp/cmp.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(TXE, LoadPNG_Test)
{
    auto cmpImporter = asset::TXEImporter::create();
    auto textureFile = cmpImporter->loadFromFile("textures/debug-empty.png");
    ASSERT_TRUE(textureFile.has_value());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}