// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/quaternion.hpp"
#include "core/vector.hpp"
#include "core/ref_ptr.hpp"

namespace ionengine
{
    class SceneNode : public core::ref_counted_object
    {
      public:
        SceneNode(std::string_view const nodeName);

        auto setPosition(core::Vec3f const& position) -> void;

        auto setScale(core::Vec3f const& scale) -> void;

        auto setRotation(core::Quatf const& rotation) -> void;

        auto getPosition() const -> core::Vec3f const&;

        auto getScale() const -> core::Vec3f const&;

        auto getRotation() const -> core::Quatf const&;

        auto addChild(core::ref_ptr<SceneNode> const& node) -> void;

        auto getName() const -> std::string const&;

        auto findInChild(std::string_view const nodeName, uint16_t const depth = 100) -> core::ref_ptr<SceneNode>;

        auto getChild() -> std::list<core::ref_ptr<SceneNode>> const&;

      private:
        std::string _nodeName;

        core::Vec3f _position;
        core::Vec3f _scale;
        core::Quatf _rotation;

        std::list<core::ref_ptr<SceneNode>> _childrens;
        std::unordered_map<std::string, SceneNode*> _nameToNode;
    };
} // namespace ionengine