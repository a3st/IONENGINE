// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "generator.hpp"
#include "graph/scene.hpp"
#include "shader_graph.hpp"

namespace ionengine::tools::editor
{
    struct ShaderGraphResult : public core::ref_counted_object
    {
        std::vector<uint8_t> outputShaderData;
    };

    class ShaderGraphEditor
    {
      public:
        ShaderGraphEditor();

        auto createAsset(ShaderGraphType const graphType) -> core::ref_ptr<ShaderGraphAsset>;

        auto loadAsset(core::ref_ptr<ShaderGraphAsset> asset) -> void;

        auto getLoadedAsset() -> core::ref_ptr<ShaderGraphAsset>;

        auto getScene() -> core::ref_ptr<Scene>;

        auto compile() -> core::ref_ptr<ShaderGraphResult>;

        auto getComponentRegistry() const -> ComponentRegistry const&;

      private:
        ComponentRegistry componentRegistry;
        core::ref_ptr<Scene> sceneGraph;
        core::ref_ptr<ShaderGraphAsset> loadedAsset;
    };
} // namespace ionengine::tools::editor