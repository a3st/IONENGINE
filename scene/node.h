// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::scene {

class Node {
public:

    virtual ~Node() = default;

    virtual void name(std::string_view const name);

    virtual std::string_view name() const;

    virtual void add_child(Node* const node);

    virtual Node* parent();

private:

    std::string _name;

    Node* _parent{nullptr};
    std::vector<Node*> _childrens;
};

}
