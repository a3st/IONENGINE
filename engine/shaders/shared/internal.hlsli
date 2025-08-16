// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

template<typename Type>
struct cbuffer_t {
    uint ptr;

    Type Get() {
        return ConstantBuffer<Type>(ResourceDescriptorHeap[NonUniformResourceIndex(ptr)]);
    }
};

struct sampler_t {
    uint ptr;

    SamplerState Get() {
        return SamplerState(SamplerDescriptorHeap[NonUniformResourceIndex(ptr)]);
    }
};

struct texture2D_t {
    uint ptr;

    Texture2D Get() {
        return Texture2D(ResourceDescriptorHeap[NonUniformResourceIndex(ptr)]);
    }
};

template<typename Type>
Type CreateResource(uint ptr) {
    Type output;
    output.ptr = ptr;
    return output;
}