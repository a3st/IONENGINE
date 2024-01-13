// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::scene {

class SceneVisitor;

class SceneNode {
public:

    virtual ~SceneNode() = default;

    virtual void name(std::string_view const name);

    virtual std::string_view name() const;

    virtual void add_child(SceneNode* const node);

    virtual SceneNode* parent();

    virtual void accept(SceneVisitor& visitor) = 0;

    virtual std::span<SceneNode*> childrens();

private:

    std::string _name;

    SceneNode* _parent{nullptr};
    std::vector<SceneNode*> _childrens;
};

}
