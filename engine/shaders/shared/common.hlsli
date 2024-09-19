// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#ifdef SHADER_DOMAIN_TYPE_SCREEN

struct VS_INPUT {
    uint id: SV_VertexID;
};

struct VS_OUTPUT {
    float4 position: SV_Position;
    float2 uv: TEXCOORD0;
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

#ifndef GetResource
#ifdef GetResource(buffer, element) ResourceDescriptorHeap[__shaderData.##buffer##Buffer].##element
#endif

#ifndef GetSampler() SamplerDescriptorHeap[__shaderData.samplerBuffer].linearSampler