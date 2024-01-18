// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "primitive.hpp"
#include "backend.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Primitive::Primitive(Backend& backend, BufferAllocator<LinearAllocator>& allocator) : backend(&backend), allocator(&allocator) {

}