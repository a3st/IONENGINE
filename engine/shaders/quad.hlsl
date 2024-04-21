
struct ShaderResources {
    uint forwardTexture;
};
ConstantBuffer<ShaderResources> shaderResources : register(b0, space0);

SamplerState static_sampler : register(s0, space0);

struct VSOutput {
    float4 position: SV_Position;
    float2 uv : TEXCOORD0;
};

VSOutput vs_main(uint vertex: SV_VertexID) {
    VSOutput output;

    output.uv = float2((vertex << 1) & 2, vertex & 2);
    output.position = float4(output.uv * 2.0f + -1.0f, 0.0f, 1.0f);
    return output;
}

struct PSOutput {
    float4 color : SV_Target0;
};

PSOutput ps_main(VSOutput input) {
    PSOutput output;

    Texture2D forwardTexture = ResourceDescriptorHeap[NonUniformResourceIndex(shaderResources.forwardTexture)];

    output.color = forwardTexture.Sample(static_sampler, input.uv);
    return output;
}

