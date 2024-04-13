
#include "precompiled.h"
#include <gtest/gtest.h>
#include "rhi/fx/dxc/compiler.hpp"

TEST(rhi_fx, read_fx_file)
{
    ionengine::rhi::fx::DXCCompiler compiler;

    compiler.add_include_path("../../engine/shaders");

    compiler.compile("../../engine/shaders/basic.fx");
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}