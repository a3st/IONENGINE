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
        Window::setEnableMouse(true);
        testCamera = Graphics::createPerspectiveCamera(68.0f, 0.1f, 100.0f);
        testCamera->setViewMatrix(core::Mat4f::lookAtRH(core::Vec3f(5.0f, 5.0f, 5.0f), core::Vec3f(0.0f, 0.0f, 0.0f),
                                                        core::Vec3f(0.0f, 0.0f, 1.0f)));
        quadShader = Graphics::loadShaderFromFile("../../assets/shaders/quad_test_color_pc.bin");
        Material::baseSurfaceMaterial =
            Graphics::createMaterial(Graphics::loadShaderFromFile("../../assets/shaders/base3d_test_color_pc.bin"));
        testMesh = Graphics::loadMeshFromFile("../../assets/models/box.mdl");

        Material::baseSurfaceMaterial->setValue("baseColor", core::Vec4f(0.9f, 0.6f, 0.9f, 1.0f));

        Graphics::setRenderPath([&]() {
            auto geometryPass = Graphics::addRenderPass<passes::GeometryPass>();
            // auto quadPass = getContext().graphics->addRenderPass<passes::QuadPass>(quadShader);
        });
    }

    auto onUpdate(float const deltaTime) -> void override
    {
    }

    auto onRender() -> void override
    {
        Graphics::drawMesh(testMesh, core::Mat4f::identity(), testCamera);
    }

  private:
    core::ref_ptr<Shader> quadShader;
    core::ref_ptr<Camera> testCamera;
    core::ref_ptr<Mesh> testMesh;
    core::ref_ptr<Material> material;
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