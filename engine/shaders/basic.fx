#include "engine.fx"

struct MaterialData {
    Texture2D diffuse;
    Texture2D normal;
    Texture2D roughness;
};

[[fx::shader_constant]] MaterialData materialData;
[[fx::shader_constant]] SamplerState linearSampler;
[[fx::shader_constant]] WorldData worldData;
[[fx::shader_constant]] LightingData lightingData[];

struct VS_INPUT {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output;
    float4 world_pos = mul(worldData.model, float4(input.position, 1.0f));
    output.position = mul(worldData.projection, mul(worldData.view, world_pos));
    output.uv = float2(input.uv.x, input.uv.y);
    return output;
}

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_OUTPUT ps_main(VS_OUTPUT input) {
    PS_OUTPUT output;
    output.color = materialData.diffuse.Sample(linearSampler, input.uv).rgba;
    return output;
}

technique {
    pass {
        vertexShader = vs_main();
        pixelShader = ps_main();
        depthWrite = false;
        stencilWrite = false;
        cullSide = "back";
    }
}