// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/handle.h>

namespace ionengine::scene {

class Node {

    friend class NodeTree;
    friend class Scene;

public:

    Node() = default;

    virtual ~Node() = default;

    virtual Node& add_child(Handle<Node> const& child);

    virtual void name(std::u8string_view const name);

    virtual std::u8string_view name() const;

    virtual Handle<Node> parent() const;

private:

    Handle<Node> _parent{INVALID_HANDLE(Node)};
    std::vector<Handle<Node>> _childrens;
    std::u8string _name;
};

}
