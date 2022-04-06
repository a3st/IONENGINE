// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/obj/obj_tree.h>

namespace ionengine::asset::obj {

class ObjDocument {
public:

    ObjDocument() = default;

    bool parse(std::span<char8_t const> const data);

private:

    ObjTree _obj_tree;
};


}