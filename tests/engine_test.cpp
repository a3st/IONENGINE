// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "engine/graphics/passes/quad.hpp"
#include "precompiled.h"
#include <gtest/gtest.h>

using namespace ionengine;

class MyEngine : public Engine
{
  public:
    MyEngine(uint32_t const argc, char** argv) : Engine(argc, argv)
    {
    }

  protected:
    auto onStart() -> void override
    {
        getContext().window->setEnableMouse(true);
    }

    auto onUpdate(float const deltaTime) -> void override
    {
    }

    auto onRender() -> void override
    {
        //auto quadPass = getContext().graphics->addRenderPass<passes::QuadPass>(quadShader);
    }

  private:
    core::ref_ptr<Shader> quadShader;
};

TEST(Engine, Instance_Test)
{
    MyEngine myEngine(__argc, __argv);
    ASSERT_EQ(myEngine.run(), EXIT_SUCCESS);
}

auto main(int32_t argc, char** argv) -> int32_t
{
    testing::InitGoogleTest(&argc, argv);
    return ::RUN_ALL_TESTS();
}