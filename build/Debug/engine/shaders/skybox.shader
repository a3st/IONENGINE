{
    name: "skybox_pc",
    uniforms: [
        {
            name: "world",
            type: "cbuffer",
            properties: [
                { name: "view", type: "float4x4" },
                { name: "proj", type: "float4x4" },
                { name: "camera_position", type: "float3" },
                { name: "inverse_view_proj", type: "float4x4" }
            ],
            visibility: "vertex"
        },
        {
            name: "object",
            type: "cbuffer",
            properties: [
                { name: "model", type: "float4x4" },
                { name: "inverse_model", type: "float4x4" },
            ],
            visibility: "vertex"
        },
        {
            name: "skybox",
            type: "samplerCube",
            visibility: "pixel"
        }
    ],
    draw_parameters: {
        fill_mode: "solid",
        cull_mode: "none",
        depth_stencil: true,
        depth_test: "less_equal",
        blend_mode: "opaque"
    },
    stages: [
        {
            type: "vertex",
            inputs: [
                { name: "position", type: "float3", semantic: "POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" },
                { name: "normal", type: "float3", semantic: "NORMAL" },
                { name: "tangent", type: "float3", semantic: "TANGENT" },
                { name: "bitangent", type: "float3", semantic: "BITANGENT" }
            ],
            outputs: [
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "uv", type: "float3", semantic: "POSITION" }
            ],
            source: "

                #define IONENGINE_MATRIX_M object.model
                #define IONENGINE_MATRIX_V world.view
                #define IONENGINE_MATRIX_P world.proj

                vs_output main(vs_input input) {
                    vs_output output;

                    float4x4 view_model =  mul(IONENGINE_MATRIX_V, IONENGINE_MATRIX_M);
                    view_model[0][3] = 0;
                    view_model[1][3] = 0;
                    view_model[2][3] = 0;

                    output.position = mul(IONENGINE_MATRIX_P, mul(view_model, float4(input.position, 1.0f))).xyww;
                    output.uv = input.position;
                    return output;
                }
            "
        },
        {
            type: "pixel",
            inputs: [
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "uv", type: "float3", semantic: "POSITION" }
            ],
            outputs: [
                { name: "color", type: "float4", semantic: "SV_TARGET0" }
            ],
            source: "

                ps_output main(ps_input input) {
                    ps_output output;
                    output.color = skybox_texture.Sample(skybox_sampler, input.uv);
                    return output;
                }
            "
        }
    ]
}
