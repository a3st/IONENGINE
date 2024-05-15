

struct ShaderResources {
    uint worldData;
    uint lightingData;
    uint materialData;
};
ConstantBuffer<ShaderResources> shaderResources : register(b0, space0);

struct WorldData {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct LightingData {
    float3 direction;
    float distance;
};

#define MAX_BONE_INFLUENCE 4

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
#ifdef SURFACE_SKINNING
    uint bones[MAX_BONE_INFLUENCE] : BLENDINDICES0;
    float weights[MAX_BONE_INFLUENCE] : BLENDWEIGHT0;
#endif
};

struct VSOutput {
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_Target0;
};

VSOutput vs_main(VSInput input) {
    
}