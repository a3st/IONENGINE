// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

class Node {
public:

    Node() = default;

    virtual ~Node() = default;

    virtual Node& add_child(Node& child);

    virtual void name(std::u8string_view const name);

    virtual std::u8string_view name() const;

protected:

    friend class Scene;

    Node* parent{nullptr};
    std::list<Node*> childrens;

    std::u8string _name;
};

}
