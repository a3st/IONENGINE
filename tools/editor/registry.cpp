// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "registry.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    auto DomainRegistry::addDomainToRegistry(std::string_view const domainType, core::ref_ptr<Node> node) -> void
    {
        domains.emplace(std::string(domainType), node);
    }

    auto DomainRegistry::dump() -> std::string
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

    auto ComponentRegistry::addComponentToRegistry(std::string_view const group, core::ref_ptr<Node> node) -> void
    {
        groups.try_emplace(std::string(group));
        groups[std::string(group)].emplace_back(node);
    }

    auto ComponentRegistry::dump() -> std::string
    {
        std::stringstream stream;
        stream << "{\"groups\":[";

        bool isFirst = true;
        for (auto const& [groupName, groupNodes] : groups)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"name\":\"" << groupName << "\",\"nodes\":[";

            bool isFirst = true;
            for (auto const& node : groupNodes)
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"name\":\"" << node->getNodeName() << "\",\"fixed\":" << node->isNodeStatic()
                       << ",\"inputs\":[";

                isFirst = true;
                for (auto const& socket : node->getInputSockets())
                {
                    if (!isFirst)
                    {
                        stream << ",";
                    }

                    stream << "{\"name\":\"" << socket.socketName << "\",\"type\":\"" << socket.socketType << "\"}";

                    isFirst = false;
                }

                stream << "],\"outputs\":[";

                isFirst = true;
                for (auto const& socket : node->getOutputSockets())
                {
                    if (!isFirst)
                    {
                        stream << ",";
                    }

                    stream << "{\"name\":\"" << socket.socketName << "\",\"type\":\"" << socket.socketType << "\"}";

                    isFirst = false;
                }

                stream << "],\"userData\":{";

                isFirst = true;
                for (auto const& [key, value] : node->getOptions())
                {
                    if (!isFirst)
                    {
                        stream << ",";
                    }

                    stream << "\"" << key << "\":\"" << value << "\"";

                    isFirst = false;
                }

                stream << "}}";

                isFirst = false;
            }
            stream << "]}";

            isFirst = false;
        }
        stream << "]}";
        return stream.str();
    }
} // namespace ionengine::tools::editor