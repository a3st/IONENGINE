
struct ShaderResources {
    uint worldData;
    uint lightingData;
    uint textureColor;
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

    ConstantBuffer<WorldData> worldData = ResourceDescriptorHeap[shaderResources.worldData];

    float4 world_pos = mul(worldData.model, float4(input.position, 1.0f));
    output.position = mul(worldData.projection, mul(worldData.view, world_pos));
    output.uv = float2(input.uv.x, 1.0f - input.uv.y);
    return output;
}

struct PSOutput {
    float4 color : SV_Target0;
};

PSOutput ps_main(VSOutput input) {
    PSOutput output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}