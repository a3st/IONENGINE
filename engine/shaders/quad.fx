
export Texture2D fullscreenTexture;
export SamplerState linearSampler;

struct VS_INPUT {
    uint id: SV_VertexID;
};

struct VS_OUTPUT {
    float4 position: SV_Position;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT input) {
    VS_OUTPUT output;
    output.uv = float2((input.id << 1) & 2, input.id & 2);
    output.position = float4(output.uv * 2.0f + -1.0f, 0.0f, 1.0f);
    return output;
}

struct PS_OUTPUT {
    float4 color : SV_Target0;
};

PS_OUTPUT ps_main(VS_OUTPUT input) {
    PS_OUTPUT output;
    output.color = fullscreenTexture.Sample(linearSampler, input.uv);
    return output;
}

technique {
    pass {
        vertexShader = vs_main();
        pixelShader = ps_main();
        cullSide = "back";
        depthWrite = false;
        stencilWrite = false;
    }
}