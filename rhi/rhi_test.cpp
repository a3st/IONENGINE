// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "rhi/rhi.hpp"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(RHI, DeviceOnly_Test)
{
    auto RHI = rhi::RHI::create(rhi::RHICreateInfo::Default());
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}