// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>

namespace ionengine::scene {

class SceneGraph {
public:

    using Iterator = std::vector<SceneNode*>::iterator;

    SceneGraph(SceneNode* root_node);

    void update_hierarchical_data();

    Iterator begin() { return _nodes.begin(); }

    Iterator end() { return _nodes.end(); }

    void visit(Iterator const& begin, Iterator const& end, SceneVisitor& visitor);

    template<class Type>
    void add_node(Type* const node) {

        _nodes.push_back(node);
    }

    SceneNode* root();

private:

    SceneNode* _root_node;
    std::vector<SceneNode*> _nodes;
};

}