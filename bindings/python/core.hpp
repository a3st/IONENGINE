// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include <pybind11/pybind11.h>

PYBIND11_DECLARE_HOLDER_TYPE(Type, ionengine::core::ref_ptr<Type>);