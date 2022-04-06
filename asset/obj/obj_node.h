// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/handle.h>

namespace ionengine::asset::obj {

class ObjNode {

    friend class ObjTree;

public:

    ObjNode() = default;

    virtual ~ObjNode() = default;

    virtual void name(std::u8string_view const name);

    virtual std::u8string_view name() const;


private:

    std::u8string _name;
};

}
