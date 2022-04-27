// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>

namespace ionengine::scene {

class SceneGraph {
public:

    using Iterator = std::vector<std::unique_ptr<SceneNode>>::iterator;

    SceneGraph();

    void update_hierarchical_data();

    Iterator begin() { return _nodes.begin(); }

    Iterator end() { return _nodes.end(); }

    void visit(Iterator const& begin, Iterator const& end, SceneVisitor& visitor);

    template<class Type>
    Type* add_node() {

        _nodes.push_back(std::make_unique<Type>());
        return static_cast<Type*>(_nodes.back().get());
    }

    SceneNode* root();

private:

    SceneNode* _root_node;

    std::vector<std::unique_ptr<SceneNode>> _nodes;
};

}