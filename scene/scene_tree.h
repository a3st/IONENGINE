// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>

namespace ionengine::scene {

class SceneTree {
public:

    using Iterator = std::vector<std::unique_ptr<SceneNode>>::iterator;
    using ConstIterator = std::vector<std::unique_ptr<SceneNode>>::const_iterator;

    SceneTree();

    Iterator begin() { return _pool.begin(); }

    Iterator end() { return _pool.end(); }

    ConstIterator begin() const { return _pool.begin(); }

    ConstIterator end() const { return _pool.end(); }

    Handle<SceneNode> root() const;

    template<class Type>
    Type& node(Handle<SceneNode> const& node) {
        
        return static_cast<Type&>(*_pool[node.id].get());
    }

    template<class Type>
    Type const& node(Handle<SceneNode> const& node) const {

        return static_cast<Type const&>(*_pool.at(node.id).get());
    }

    template<class Type>
    Handle<SceneNode> spawn_node(std::u8string_view const name, Handle<SceneNode> const& parent = INVALID_HANDLE(SceneNode)) {

        uint32_t const id = static_cast<uint32_t>(_pool.size());
        _pool.push_back(std::make_unique<Type>());
        _pool.back()->name(name);

        if(parent != INVALID_HANDLE(SceneNode)) {

            _pool[parent.id]->add_child(id);
            _pool.back()->_parent = parent;
        }

        return id;
    }

    template<class Type>
    void dfs(std::span<uint32_t> const groups, std::span<Handle<SceneNode>> const nodes) {

        std::stack<std::pair<uint32_t, Handle<SceneNode>>> next_nodes;
        uint32_t cur_node_index = 0;

        next_nodes.emplace(0, _root);

        while(!next_nodes.empty()) {

            auto cur_node = next_nodes.top();
            next_nodes.pop();

            uint32_t const child_size = static_cast<uint32_t>(_pool[cur_node.second.id]->_childrens.size());

            for(size_t i = 0; i < child_size; ++i) {

                uint32_t const cur_group_index = cur_node.first + 1;
                next_nodes.emplace(cur_group_index, _pool[cur_node.second.id]->_childrens[i]);

                Type* node_casted = dynamic_cast<Type*>(_pool[_pool[cur_node.second.id]->_childrens[i].id].get());

                if(node_casted) {
                    nodes[cur_node_index] = Handle<SceneNode>(_pool[cur_node.second.id]->_childrens[i].id);
                    ++groups[cur_group_index - 1];
                    ++cur_node_index;
                }
            }
        }
    }

private:

    Handle<SceneNode> _root;
    std::vector<std::unique_ptr<SceneNode>> _pool;
};

}
