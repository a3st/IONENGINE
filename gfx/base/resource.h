// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

/**
    @brief Graphics API Resource class
 
    Resource class for Graphics API. It's needed for safe gpu resource operations
*/
class Resource {
public:

    virtual ~Resource() = default;

    /**
        @brief Bind the memory to this resource
        @param memory reference to Memory object
        @param offset bind offset to memory
    */
    virtual void bind_memory(Memory& memory, const uint64 offset) = 0;

    /**
        @brief Get the type of resource
        @return ResourceType type of resource
    */
    virtual ResourceType get_type() const = 0;

    /**
        @brief Get the description of resource
        @return const std::variant<ResourceDesc, SamplerDesc>& get description of resource (ResourceDesc or SamplerDesc)
    */
    virtual const std::variant<ResourceDesc, SamplerDesc>& get_desc() const = 0;

    /**
        @brief Map resource to host 
        @return byte* pointer to begin data
    */
    virtual byte* map() = 0;

    /**
        @brief Unmap resource from host
    */
    virtual void unmap() = 0;
};

}