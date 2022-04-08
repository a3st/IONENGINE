// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/handle.h>

namespace ionengine::scene {

class SceneNode {

    friend class SceneTree;

public:

    SceneNode() = default;

    virtual ~SceneNode() = default;

    virtual void add_child(Handle<SceneNode> const& child);

    virtual void name(std::string_view const name);

    virtual std::string_view name() const;

    virtual Handle<SceneNode> parent() const;

private:

    Handle<SceneNode> _parent{INVALID_HANDLE(SceneNode)};
    std::vector<Handle<SceneNode>> _childrens;
    std::string _name;
};

}
