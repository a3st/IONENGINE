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
    float v;
    sampler_t linearSampler;
};

struct SHADER_DATA {
    cbuffer_t<MATERIAL_DATA> materialBuffer;
    cbuffer_t<SAMPLER_DATA> samplerBuffer;
};

ConstantBuffer<SHADER_DATA> gShaderData : register(b0, space0);
