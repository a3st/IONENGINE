
struct ShaderResources {
    uint worldData;
    uint lightingData;
    uint textureColor;
};

struct WorldData {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct LightingData {
    float3 direction;
    float distance;
};

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VSOutput {
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

VSOutput vs_main(VSInput input) {
    VSOutput output;
    return output;
}

struct PSOutput {
    float4 color : SV_Target0;
};

PSOutput ps_main(VSOutput input) {
    PSOutput output;
    return output;
}