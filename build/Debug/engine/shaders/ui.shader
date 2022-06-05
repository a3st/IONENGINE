{
    name: "ui_pc",
    uniforms: [
        {
            name: "ui_element",
            type: "cbuffer",
            properties: [
                { name: "projection", type: "float4x4" },
                { name: "transform", type: "float4x4" },
                { name: "translation", type: "float2" },
                { name: "has_texture", type: "_bool" }
            ],
            visibility: "all"
        },
        {
            name: "albedo",
            type: "sampler2D",
            visibility: "pixel"
        }
    ],
    draw_parameters: {
        fill_mode: "solid",
        cull_mode: "none",
        depth_stencil: false,
        blend: true
    },
    stages: [
        {
            type: "vertex",
            inputs: [
                { name: "position", type: "float2", semantic: "POSITION" },
                { name: "color", type: "float4", semantic: "COLOR" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "color", type: "float4", semantic: "COLOR" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            source: "

                vs_output main(vs_input input) {
                    vs_output output;

                    float4 document_position = float4(input.position + ui_element.translation, 0.0f, 1.0f);
                    output.position = mul(ui_element.projection, mul(ui_element.transform, document_position));
                    output.color = input.color;
                    output.uv = float2(input.uv.x, input.uv.y);
                    return output;
                }
            "
        },
        {
            type: "pixel",
            inputs: [
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "color", type: "float4", semantic: "COLOR" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "color", type: "float4", semantic: "SV_TARGET0" }
            ],
            source: "

                ps_output main(ps_input input) {
                    ps_output output;
                    if(ui_element.has_texture) {
                        float4 albedo = albedo_texture.Sample(albedo_sampler, input.uv);
                        output.color = input.color * albedo;
                    } else {
                        output.color = input.color;
                    }
                    return output;
                }
            "
        }
    ]
}
