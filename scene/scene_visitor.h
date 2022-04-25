// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::scene {

class MeshNode;
class TransformNode;

class SceneVisitor {
public:

    virtual void operator()(MeshNode& other) = 0;
    virtual void operator()(TransformNode& other) = 0;
};

}
