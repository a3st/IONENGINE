import = ( 
    "engine",
    "stdlib" 
)

fullscreenTexture: Texture2D fx::constant(0);
linearSampler: SamplerState fx::constant(1);

VS_INPUT: struct = {
    id: uint SV_VertexID;
}

VS_OUTPUT: struct = {
    position: float4 SV_Position;
    uv: float2 TEXCOORD0;
}

vs_main: (input: VS_INPUT) -> VS_OUTPUT = {
    output: VS_OUTPUT = (
        float2((input.id << 1) & 2, input.id & 2),
        float4(output.uv * 2.0f + -1.0f, 0.0f, 1.0f)
    );
    return output;
}

PS_OUTPUT: struct = {
    color: float4 SV_Target0;
}

ps_main: (input: VS_OUTPUT) -> PS_OUTPUT {
    output: PS_OUTPUT = (
        fullscreenTexture.Sample(linearSampler, input.uv).rgba
    );
    return output;
}

technique = {
    vertexShader = vs_main;
    pixelShader = ps_main;
    cullSide = "back";
    depthWrite = false;
    stencilWrite = false;
}