#include "engine.fx"

struct MaterialData {
    Texture2D diffuse;
    Texture2D normal;
    Texture2D roughness;
};

export MaterialData materialData;
export SamplerState linearSampler;
export WorldData worldData;
export LightingData lightingData[];

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

// C помощью регулярки найти границы функции
// В ограниченной области найти все exports по очереди
// Если export найден (и это ConstantBuffer), pos на !worldData. 
// Находим переменную после . и проверяем ее тип, если Texture то обращаемся через 
//      Texture2D(ResourceDescriptorHeap[NonUniformResourceIndex(ConstantBuffer<ConstName>(ResourceDescriptorHeap[shaderData.worldData]).name)]
// Если нет то
//      ConstantBuffer<ConstName>(ResourceDescriptorHeap[shaderData.worldData]).name

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output;
    /*float4 world_pos = mul(worldData.model, float4(input.position, 1.0f));
    output.position = mul(worldData.projection, mul(worldData.view, world_pos));
    output.uv = float2(input.uv.x, input.uv.y);*/
    float4 world_pos = mul(ConstantBuffer<WorldData>(ResourceDescriptorHeap[shaderData.worldData]).model, float4(input.position, 1.0f));
    output.position = mul(ConstantBuffer<WorldData>(ResourceDescriptorHeap[shaderData.worldData]).projection, 
        mul(ConstantBuffer<WorldData>(ResourceDescriptorHeap[shaderData.worldData]).view, world_pos));
    output.uv = float2(input.uv.x, input.uv.y);
    return output;
} 

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_OUTPUT ps_main(VS_OUTPUT input) {
    PS_OUTPUT output;
    Texture2D color = Texture2D(ResourceDescriptorHeap[NonUniformResourceIndex(ConstantBuffer<MaterialData>(ResourceDescriptorHeap[shaderData.materialData]).diffuse)]);
    output.color = float4(0.2f, 0.3f, 0.1f, 1.0f);
    return output;
};

technique {
    pass {
        vertexShader = vs_main();
        pixelShader = ps_main();
        depthWrite = false;
        stencilWrite = false;
        cullSide = "back";
    }
}