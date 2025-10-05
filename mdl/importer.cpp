// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "importer.hpp"
#include "obj/obj.hpp"
#include "precompiled.h"

namespace ionengine::asset
{
    auto MDLImporter::create() -> core::ref_ptr<MDLImporter>
    {
        return core::make_ref<OBJImporter>();
    }
} // namespace ionengine::asset