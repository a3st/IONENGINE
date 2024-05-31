// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "merger.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    auto Merger::addComponentToMerger(std::string_view const domainType, core::ref_ptr<Node> node) -> void
    {
        domains.emplace(std::string(domainType), node);
    }

    auto Merger::dump() -> std::string
    {
        std::stringstream stream;
        stream << "{\"domains\":[";

        bool isFirst = true;
        for (auto const& [domainName, domainNode] : domains)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"name\":\"" << domainName << "\",\"node\":" << domainNode->dump() << "}";
            isFirst = false;
        }
        stream << "]}";
        return stream.str();
    }
} // namespace ionengine::tools::editor