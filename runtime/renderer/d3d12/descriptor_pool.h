// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class DesctiptorType {
    Sampler,
    Resource,
    Buffer,
    UnorderedAccess
};

template<DesctiptorType T>
class DescriptorPool {
};

template<>
class DescriptorPool<DesctiptorType::Sampler> {
public:

    DescriptorPool() {

    }

private:

};

template<>
class DescriptorPool<DesctiptorType::Resource> {
public:

    DescriptorPool() {

    }

private:

};

template<>
class DescriptorPool<DesctiptorType::Buffer> {
public:

    DescriptorPool() {

    }

private:

};

template<>
class DescriptorPool<DesctiptorType::UnorderedAccess> {
public:

    DescriptorPool() {

    }

private:

};
    
}