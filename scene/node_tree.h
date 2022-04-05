// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/node.h>
#include <lib/handle.h>

namespace ionengine::scene {

class NodeTree {
public:

    using Iterator = std::vector<Node>::iterator;
    using ConstIterator = std::vector<Node>::const_iterator;

    NodeTree();

    Iterator begin() { return _pool.begin(); }

    Iterator end() { return _pool.end(); }

    ConstIterator begin() const { return _pool.begin(); }

    ConstIterator end() const { return _pool.end(); }

    Handle<Node> root() const;

    template<class Type>
    Type& node(Handle<Node> const& node) {
        
        return static_cast<Type&>(_pool[node.id]);
    }

    template<class Type>
    Type const& node(Handle<Node> const& node) const {

        return static_cast<Type const&>(_pool.at(node.id));
    }

    template<class Type>
    Handle<Node> spawn_node(std::u8string_view const name, Handle<Node> const& parent = INVALID_HANDLE(Node)) {

        uint32_t const id = static_cast<uint32_t>(_pool.size());

        auto& _node = _pool.emplace_back(Type());
        _node.name(name);

        if(parent != INVALID_HANDLE(Node)) {
            auto& _parent = node<Node>(parent);
            _parent.add_child(id);
            _node._parent = parent;
        }

        return id;
    }

private:

    Handle<Node> _root;
    std::vector<Node> _pool;
};

}
