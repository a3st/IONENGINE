// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#ifdef SHADER_DOMAIN_TYPE_SCREEN

struct VS_INPUT {
    uint id: SV_VertexID;
};

struct VS_OUTPUT {
    float4 position: SV_Position;
    float2 uv: TEXCOORD0;
};

struct PS_OUTPUT {
    float4 color: SV_Target0;
};

#endif

struct SAMPLER_DATA {
    uint linearSampler;
};

struct SHADER_DATA {
    uint materialBuffer;
    uint samplerBuffer;
};

ConstantBuffer<SHADER_DATA> __shaderData : register(b0, space0);

#ifndef GetBuffer
#define GetBuffer(type, buffer) ConstantBuffer<type>(ResourceDescriptorHeap[__shaderData. buffer])
#endif

#ifndef GetTexture
#define GetTexture(type, buffer, texture) ResourceDescriptorHeap[ConstantBuffer<type>(ResourceDescriptorHeap[__shaderData. buffer]). texture]
#endif

#ifndef GetSampler
#define GetSampler() SamplerDescriptorHeap[ConstantBuffer<SAMPLER_DATA>(ResourceDescriptorHeap[__shaderData.samplerBuffer]).linearSampler]
#endif
