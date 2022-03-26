// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/node.h>

#include <scene/resources/material.h>
#include <scene/resources/texture.h>

namespace ionengine::scene {

using Resource = std::variant<resources::Texture, resources::Material>;

class ResourceNode : public Node {
public:

    ResourceNode() {}

    virtual ~ResourceNode() { }

    virtual Node& add_child(Node& child);

    virtual void name(std::u8string_view const name);

    virtual std::u8string_view name() const;

    void resource(Resource const& resource);

private:

    std::optional<Resource> _resource;
};

}
