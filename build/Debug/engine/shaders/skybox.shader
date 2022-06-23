{
    name: "skybox_pc",
    conditions: [],
    uniforms: [
        {
            name: "world",
            type: "cbuffer",
            data: [
                { name: "view", type: "f32x4x4" },
                { name: "proj", type: "f32x4x4" },
                { name: "camera_position", type: "f32x3" }
            ],
            visibility: "vertex"
        },
        {
            name: "object",
            type: "cbuffer",
            data: [
                { name: "model", type: "f32x4x4" },
                { name: "inverse_model", type: "f32x4x4" }
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
        depth_test: "less_equal",
        blend_mode: "opaque"
    },
    stages: [
        {
            type: "vertex",
            inputs: [
                { name: "position", type: "f32x3", semantic: "POSITION" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" },
                { name: "normal", type: "f32x3", semantic: "NORMAL" },
                { name: "tangent", type: "f32x3", semantic: "TANGENT" },
                { name: "bitangent", type: "f32x3", semantic: "BITANGENT" }
            ],
            outputs: [
                { name: "position", type: "f32x4", semantic: "SV_POSITION" },
                { name: "uv", type: "f32x3", semantic: "POSITION" }
            ],
            source: "

                #define IONENGINE_MATRIX_M object.model
                #define IONENGINE_MATRIX_V world.view
                #define IONENGINE_MATRIX_P world.proj

                vs_output main(vs_input input) {
                    vs_output output;

                    f32x4x4 view_model =  mul(IONENGINE_MATRIX_V, IONENGINE_MATRIX_M);
                    view_model[0][3] = 0;
                    view_model[1][3] = 0;
                    view_model[2][3] = 0;

                    output.position = mul(IONENGINE_MATRIX_P, mul(view_model, f32x4(input.position, 1.0f))).xyww;
                    output.uv = input.position;
                    return output;
                }
            "
        },
        {
            type: "pixel",
            inputs: [
                { name: "position", type: "f32x4", semantic: "SV_POSITION" },
                { name: "uv", type: "f32x3", semantic: "POSITION" }
            ],
            outputs: [
                { name: "color", type: "f32x4", semantic: "SV_TARGET0" }
            ],
            source: "

                ps_output main(ps_input input) {
                    ps_output output;
                    output.color = TEXTURE_SAMPLE(skybox, input.uv);
                    return output;
                }
            "
        }
    ]
}
