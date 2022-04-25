// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <scene/scene_graph.h>

namespace ionengine::scene {

class Scene {
public:

    using Iterator = std::vector<std::unique_ptr<SceneNode>>::iterator;

    Scene();

    Iterator begin() { return _nodes.begin(); }

    Iterator end() { return _nodes.end(); }

    void visit(Iterator const& begin, Iterator const& end, SceneVisitor& visitor);

    template<class Type>
    void push_node() {

        _nodes.push_back(std::make_unique<Type>());
    }

private:

    SceneGraph _scene_graph;

    std::vector<std::unique_ptr<SceneNode>> _nodes;
};

}
