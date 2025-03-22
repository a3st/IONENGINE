// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine.hpp"
#include "engine/graphics/passes/geometry.hpp"
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
        testCamera = getContext().graphics->createPerspectiveCamera(68.0f, 0.1f, 100.0f);
        testCamera->setViewMatrix(core::Mat4f::lookAtRH(core::Vec3f(5.0f, 5.0f, 5.0f), core::Vec3f(0.0f, 0.0f, 0.0f),
                                                        core::Vec3f(0.0f, 0.0f, 1.0f)));
        quadShader = getContext().graphics->loadShaderFromFile("../../assets/shaders/quad_test_color_pc.bin");
        testMesh = getContext().graphics->loadMeshFromFile("../../assets/models/box.mdl");

        getContext().graphics->renderPathUpdated = [&]() {
            auto geometryPass = getContext().graphics->addRenderPass<passes::GeometryPass>();
            // auto quadPass = getContext().graphics->addRenderPass<passes::QuadPass>(quadShader);
        };
    }

    auto onUpdate(float const deltaTime) -> void override
    {
    }

    auto onRender() -> void override
    {
        getContext().graphics->drawMesh(testMesh, core::Mat4f::identity(), testCamera);
    }

  private:
    core::ref_ptr<Shader> quadShader;
    core::ref_ptr<Camera> testCamera;
    core::ref_ptr<Mesh> testMesh;
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