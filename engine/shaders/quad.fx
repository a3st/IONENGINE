import = ( 
    "engine",
    "std" 
)

fullscreenTexture: Texture2D constant(0);
linearSampler: SamplerState constant(1);

VS_INPUT: struct = {
    id: uint SV_VertexID;
}

VS_OUTPUT: struct = {
    position: float4 semantic(SV_Position);
    uv: float2 semantic(TEXCOORD0);
}

vs_main: (input: VS_INPUT) -> VS_OUTPUT = {
    output: VS_OUTPUT = (
        float4(output.uv * 2.0 + -1.0, 0.0, 1.0),
        float2((input.id << 1) & 2, input.id & 2)
    );
    return output;
}

PS_OUTPUT: struct = {
    color: float4 semantic(SV_Target0);
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