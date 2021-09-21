// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class DescriptorSetLayout  {
public:

    virtual ~DescriptorSetLayout() = default;
};

class DescriptorPool  {
public:

    virtual ~DescriptorPool() = default;

};

class DescriptorSet {
public:

    virtual ~DescriptorSet() = default;
    
};

}