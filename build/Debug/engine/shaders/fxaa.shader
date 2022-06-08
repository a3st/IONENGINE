{
    name: "fxaa_pc",
    uniforms: [
        {
            name: "color",
            type: "sampler2D",
            visibility: "pixel"
        }
    ],
    draw_parameters: {
        fill_mode: "solid",
        cull_mode: "back",
        depth_stencil: false,
        blend: false
    },
    stages: [
        {
            type: "vertex",
            inputs: [
                { name: "position", type: "float3", semantic: "POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            source: "
                vs_output main(vs_input input) {
                    vs_output output;
                    output.position = float4(input.position, 1.0f);
                    output.uv = float2(input.uv.x, 1.0f - input.uv.y);
                    return output;
                }
            "
        },
        {
            type: "pixel",
            inputs: [
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "color", type: "float4", semantic: "SV_TARGET0" }
            ],
            source: "

                ps_output main(ps_input input) {
                    ps_output output;

                    uint width, height, levels;
                    color_texture.GetDimensions(0, width, height, levels); 
                    float2 resolution = float2(width, height);

                    float FXAA_SPAN_MAX = 8.0;
                    float FXAA_REDUCE_MUL = 1.0 / 8.0;
                    float FXAA_REDUCE_MIN = 1.0 / 128.0;
                    
                    float3 rgbNW = color_texture.Sample(color_sampler, input.uv + float2(-1.0, -1.0) / resolution, 0).rgb;
                    float3 rgbNE = color_texture.Sample(color_sampler, input.uv + float2(1.0, -1.0) / resolution, 0).rgb;
                    float3 rgbSW = color_texture.Sample(color_sampler, input.uv + float2(-1.0, 1.0) / resolution, 0).rgb;
                    float3 rgbSE = color_texture.Sample(color_sampler, input.uv + float2(1.0, 1.0) / resolution, 0).rgb;
                    float3 rgbM  = color_texture.Sample(color_sampler, input.uv, 0).rgb;
                    
                    float3 luma = float3(0.299, 0.587, 0.114);
                    float lumaNW = dot(rgbNW, luma);
                    float lumaNE = dot(rgbNE, luma);
                    float lumaSW = dot(rgbSW, luma);
                    float lumaSE = dot(rgbSE, luma);
                    float lumaM  = dot(rgbM, luma);
                    
                    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
                    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
                    
                    float2 dir;
                    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
                    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
                    
                    float dirReduce = max(
                        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
                        FXAA_REDUCE_MIN);
                    
                    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
                    
                    dir = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
                        max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
                        dir * rcpDirMin)) / resolution;
                    
                    float3 rgbA = (1.0 / 2.0) *
                    (
                        color_texture.Sample(color_sampler, input.uv + dir * (1.0 / 3.0 - 0.5), 0).rgb +
                        color_texture.Sample(color_sampler, input.uv + dir * (2.0 / 3.0 - 0.5), 0).rgb
                    );
                    
                    float3 rgbB = rgbA * (1.0 / 2.0) + (1.0 / 4.0) *
                    (
                        color_texture.Sample(color_sampler, input.uv + dir * (0.0 / 3.0 - 0.5), 0).rgb +
                        color_texture.Sample(color_sampler, input.uv + dir * (3.0 / 3.0 - 0.5), 0).rgb
                    );
                    
                    float lumaB = dot(rgbB, luma);

                    float3 finalColor;
                    if ((lumaB < lumaMin) || (lumaB > lumaMax))
                    {
                        finalColor.xyz = rgbA;
                    }
                    else
                    {
                        finalColor.xyz = rgbB;
                    }
                    
                    output.color = float4(finalColor, 1.0f);
                    return output;
                }
            "
        }
    ]
}
