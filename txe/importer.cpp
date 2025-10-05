// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "importer.hpp"
#include "cmp/cmp.hpp"
#include "precompiled.h"

namespace ionengine::asset
{
    auto TXEImporter::create() -> core::ref_ptr<TXEImporter>
    {
        return core::make_ref<CMPImporter>(true);
    }
} // namespace ionengine::asset