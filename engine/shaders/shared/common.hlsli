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
#elif SHADER_DOMAIN_TYPE_SURFACE
struct VS_INPUT {
    uint id: SV_VertexID;
};

struct TRANSFORM_DATA {
    float4x4 modelViewProj;
};
#endif

struct SAMPLER_DATA {
    sampler_t linearSampler;
};

struct SHADER_DATA {
#ifdef SHADER_DOMAIN_TYPE_SURFACE
    cbuffer_t<TRANSFORM_DATA> transformBuffer;
#endif
    cbuffer_t<SAMPLER_DATA> samplerBuffer;
    cbuffer_t<MATERIAL_DATA> materialBuffer;
};

ConstantBuffer<SHADER_DATA> gShaderData : register(b0, space0);
