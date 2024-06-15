// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graph/node.hpp"

namespace ionengine::tools::editor
{
    class ComponentRegistry
    {
      public:
        ComponentRegistry() = default;

        template <typename Type = NodeComponent>
        auto registerComponent() -> void
        {
            auto component = core::make_ref<Type>();
            addComponentToRegistry(component);
        }

        auto getComponents() -> std::unordered_map<uint32_t, core::ref_ptr<NodeComponent>> const&;

        template <typename Type = NodeComponent>
        auto getComponentByType() -> core::ref_ptr<NodeComponent>
        {
            std::string const fullClassName(typeid(Type).name());
            std::string const className(fullClassName.begin() + fullClassName.find_last_of("::") + 1,
                                        fullClassName.begin() + fullClassName.find("_"));
            return getComponentFromRegistryByType(core::crc32(className));
        }

      private:
        std::unordered_map<uint32_t, core::ref_ptr<NodeComponent>> components;

        auto addComponentToRegistry(core::ref_ptr<NodeComponent> component) -> void;

        auto getComponentFromRegistryByType(uint32_t const componentID) -> core::ref_ptr<NodeComponent>;
    };
} // namespace ionengine::tools::editor