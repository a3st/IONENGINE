// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "scene_node.hpp"
#include "precompiled.h"

namespace ionengine
{
    SceneNode::SceneNode(std::string_view const nodeName) : _nodeName(nodeName)
    {
    }

    auto SceneNode::setPosition(core::Vec3f const& position) -> void
    {
        _position = position;
    }

    auto SceneNode::setScale(core::Vec3f const& scale) -> void
    {
        _scale = scale;
    }

    auto SceneNode::setRotation(core::Quatf const& rotation) -> void
    {
        _rotation = rotation;
    }

    auto SceneNode::getPosition() const -> core::Vec3f const&
    {
        return _position;
    }

    auto SceneNode::getScale() const -> core::Vec3f const&
    {
        return _scale;
    }

    auto SceneNode::getRotation() const -> core::Quatf const&
    {
        return _rotation;
    }

    auto SceneNode::getName() const -> std::string const&
    {
        return _nodeName;
    }

    auto SceneNode::addChild(core::ref_ptr<SceneNode> const& node) -> void
    {
        auto it = _nameToNode.find(node->getName());
        if (it == _nameToNode.end())
        {
            _childrens.emplace_back(node);
            _nameToNode.emplace(node->getName(), node.get());
        }
    }

    auto SceneNode::getChild() -> std::list<core::ref_ptr<SceneNode>> const&
    {
        return _childrens;
    }

    auto SceneNode::findInChild(std::string_view const nodeName, uint16_t const depth) -> core::ref_ptr<SceneNode>
    {
        if (depth == 0)
        {
            return nullptr;
        }

        std::stack<std::pair<SceneNode*, uint16_t>> bypassOrder;
        bypassOrder.push({this, 0});

        while (!bypassOrder.empty())
        {
            auto [currentNode, currentDepth] = bypassOrder.top();
            bypassOrder.pop();

            if (currentDepth > depth)
            {
                continue;
            }

            if (currentNode->getName() == nodeName)
            {
                return currentNode;
            }

            for (auto child : currentNode->_childrens)
            {
                bypassOrder.push({child.get(), currentDepth + 1});
            }
        }
        return nullptr;
    }
} // namespace ionengine