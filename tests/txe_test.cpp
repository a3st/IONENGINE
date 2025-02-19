// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "txe/png/png.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(TXE, LoadPNG_Test)
{
    auto pngImporter = core::make_ref<asset::PNGImporter>();
    std::string errors;
    auto textureFile = pngImporter->loadFromFile("../../engine/textures/spngbob.png", errors);
    std::cout << errors << std::endl;
    ASSERT_TRUE(textureFile.has_value());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}