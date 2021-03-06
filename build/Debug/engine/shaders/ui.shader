{
    name: "ui_pc",
    conditions: [],
    uniforms: [
        {
            name: "ui_element",
            type: "cbuffer",
            data: [
                { name: "projection", type: "f32x4x4" },
                { name: "transform", type: "f32x4x4" },
                { name: "translation", type: "f32x2" },
                { name: "has_texture", type: "boolean" }
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
        depth_test: "none",
        blend_mode: "alpha_blend"
    },
    stages: [
        {
            type: "vertex",
            inputs: [
                { name: "position", type: "f32x2", semantic: "POSITION" },
                { name: "color", type: "f32x4", semantic: "COLOR" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "position", type: "f32x4", semantic: "SV_POSITION" },
                { name: "color", type: "f32x4", semantic: "COLOR" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" }
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
                { name: "position", type: "f32x4", semantic: "SV_POSITION" },
                { name: "color", type: "f32x4", semantic: "COLOR" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "color", type: "f32x4", semantic: "SV_TARGET0" }
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
