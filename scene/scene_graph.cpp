// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene_graph.h>
#include <scene/transform_node.h>

using namespace ionengine::scene;

SceneGraph::SceneGraph(SceneNode* root_node) : 
    _root_node(root_node) {

}

void SceneGraph::visit(Iterator const& begin, Iterator const& end, SceneVisitor& visitor) {

    std::for_each(
        begin, 
        end, 
        [&](auto& element) {
            element->accept(visitor);
        }
    );
}

void SceneGraph::update_hierarchical_data() {

    std::function<void(SceneNode* const)> update_recursively = [&](SceneNode* const node) {

        if(!dynamic_cast<TransformNode*>(node)) {
            return;
        }

        auto transform_node = static_cast<TransformNode*>(node);

        transform_node->_model_local = 
            transform_node->_rotation.to_matrix() *
            lib::math::Matrixf::translate(transform_node->_position) *
            lib::math::Matrixf::scale(transform_node->_scale)
        ;

        lib::math::Matrixf parent_global_transform;

        if(transform_node->parent() != nullptr) {
            auto parent_transform_node = static_cast<TransformNode*>(transform_node->parent());
            parent_global_transform = parent_transform_node->_model_global;
        } else {
            parent_global_transform = transform_node->_model_local;
        }

        lib::math::Matrixf new_global_transform = transform_node->_model_local * parent_global_transform;

        transform_node->_model_global = new_global_transform;

        std::for_each(
            transform_node->childrens().begin(), 
            transform_node->childrens().end(), 
            [&](auto& element) {
                update_recursively(element);
            }
        );
    };
    
    update_recursively(_root_node);
}

SceneNode* SceneGraph::root() {

    return _root_node;
}
