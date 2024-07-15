#include "platform/platform.hpp"
#include "precompiled.h"
#include "rhi/rhi.hpp"
#include <gtest/gtest.h>

using namespace ionengine;

TEST(RHI, DeviceOffRender_Test)
{
    rhi::RHICreateInfo rhiCreateInfo{.window = nullptr};

    auto device = rhi::Device::create(rhiCreateInfo);
}

TEST(RHI, DeviceSwapchain_Test)
{
    auto platform = platform::App::create("TestProject");

    rhi::RHICreateInfo rhiCreateInfo{.window = platform->getWindowHandle(),
                                     .instance = platform->getInstanceHandle(),
                                     .windowWidth = 800,
                                     .windowHeight = 600};

    auto device = rhi::Device::create(rhiCreateInfo);

    platform->run();
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}