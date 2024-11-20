// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "compiler.hpp"
#include "precompiled.h"

#ifdef IONENGINE_SHADERSYS_DXC
#include "shadersys/dxc/dxc.hpp"
#endif

namespace ionengine::shadersys
{
    auto getAllVariants(std::span<std::string const> const permutations,
                        std::unordered_map<std::string, uint32_t> const& flags) -> std::vector<uint32_t>
    {
        std::vector<uint32_t> variants = {1};

        uint32_t flag = 0;
        for (size_t const i : std::views::iota(0u, permutations.size()))
        {
            flag |= flags.at(permutations[i]);

            for (size_t const j : std::views::iota(i + 1, permutations.size()))
            {
                uint32_t const finalFlag = flag | flags.at(permutations[j]);
                variants.emplace_back(finalFlag);
            }
        }

        return variants;
    }

    auto ShaderCompiler::create(asset::fx::APIType const apiType) -> core::ref_ptr<ShaderCompiler>
    {
#ifdef IONENGINE_SHADERSYS_DXC
        return core::make_ref<DXCCompiler>(apiType);
#else
#error shader system backend is not defined
#endif
    }
} // namespace ionengine::shadersys