// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/crc32.hpp"
#include "core/ref_ptr.hpp"

namespace ionengine::tools::editor
{
    struct Node : public core::ref_counted_object
    {
        struct SocketInfo
        {
            std::string socketName;
            std::string socketType;
        };

        Node(std::string_view const nodeName, uint32_t const componentID);

        uint64_t nodeID;
        std::string nodeName;
        int32_t posX;
        int32_t posY;
        std::vector<SocketInfo> inputs;
        std::vector<SocketInfo> outputs;
        std::unordered_map<std::string, std::string> options;
        uint32_t componentID;
    };

    class NodeComponent : public core::ref_counted_object
    {
      public:
        NodeComponent(std::string_view const componentName, bool const contextRegister,
                      std::optional<std::string_view> const groupName, const bool fixed);

        virtual auto create(uint64_t const nodeID) -> core::ref_ptr<Node> = 0;

        virtual auto setInputs() const -> std::vector<Node::SocketInfo>
        {
            return {};
        }

        virtual auto setOutputs() const -> std::vector<Node::SocketInfo>
        {
            return {};
        }

        virtual auto setOptions() const -> std::unordered_map<std::string, std::string>
        {
            return {};
        }

        auto getName() const -> std::string_view;

        auto getGroupName() const -> std::optional<std::string_view>;

        auto isContextRegister() const -> bool;

        auto isFixed() const -> bool;

        virtual auto generateInitialShaderCode(Node const& node) const -> std::string
        {
            return "";
        }

        virtual auto generateResourceShaderCode(Node const& node) const -> std::string
        {
            return "";
        }

        virtual auto generateComputeShaderCode(Node const& node) const -> std::string
        {
            return "";
        }

        uint32_t componentID;

      private:
        std::string componentName;
        std::optional<std::string> groupName;
        bool contextRegister;
        bool fixed;
    };

#ifndef IONENGINE_NODE_COMPONENT_BEGIN
#define IONENGINE_NODE_COMPONENT_BEGIN(ComponentClass, ComponentName, ContextRegister, GroupName, Fixed)               \
    struct ComponentClass##_Node : public Node                                                                         \
    {                                                                                                                  \
        ComponentClass##_Node(uint64_t const nodeID) : Node(ComponentName, core::crc32(#ComponentClass))               \
        {                                                                                                              \
            this->nodeID = nodeID;                                                                                     \
        }                                                                                                              \
    };                                                                                                                 \
                                                                                                                       \
    class ComponentClass##_NodeComponent : public NodeComponent                                                        \
    {                                                                                                                  \
      public:                                                                                                          \
        ComponentClass##_NodeComponent() : NodeComponent(ComponentName, ContextRegister, GroupName, Fixed)             \
        {                                                                                                              \
            this->componentID = core::crc32(#ComponentClass);                                                          \
        }                                                                                                              \
                                                                                                                       \
        auto create(uint64_t const nodeID) -> core::ref_ptr<Node> override                                             \
        {                                                                                                              \
            auto nodeInstance = core::make_ref<ComponentClass##_Node>(nodeID);                                         \
            nodeInstance->inputs = this->setInputs();                                                                  \
            nodeInstance->outputs = this->setOutputs();                                                                \
            nodeInstance->options = this->setOptions();                                                                \
            return nodeInstance;                                                                                       \
        }
#endif

#ifndef IONENGINE_NODE_COMPONENT_END
#define IONENGINE_NODE_COMPONENT_END                                                                                   \
    }                                                                                                                  \
    ;
#endif
} // namespace ionengine::tools::editor