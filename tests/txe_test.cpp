// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "txe/cmp/cmp.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(TXE, LoadPNG_Test)
{
    auto cmpImporter = core::make_ref<asset::CMPImporter>(false);
    auto textureFile = cmpImporter->loadFromFile("../../engine/textures/spngbob.png");
    ASSERT_TRUE(textureFile.has_value());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}