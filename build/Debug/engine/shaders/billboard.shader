{
    name: "billboard",
    uniforms: [
        {
            name: "world",
            type: "cbuffer",
            data: [
                { name: "view", type: "float4x4" },
                { name: "proj", type: "float4x4" },
                { name: "camera_position", type: "float3" },
                { name: "inverse_view_proj", type: "float4x4" }
            ],
            visibility: "vertex"
        },
        {
            name: "color",
            type: "sampler2D",
            visibility: "pixel"
        },
        { 
            name: "object", 
            type: "sbuffer", 
            data: [
                { name: "model", type: "f32x4x4" },
                { name: "inverse_model", type: "f32x4x4" }
            ],
            visibility: "vertex"
        }
    ],
    draw_parameters: {
        fill_mode: "solid",
        cull_mode: "none",
        depth_stencil: false,
        depth_test: "less",
        blend_mode: "opaque"
    },
    stages: [
        {
            type: "vertex",
            inputs: [
                { name: "position", type: "f32x3", semantic: "POSITION" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "position", type: "f32x4", semantic: "SV_POSITION" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" }
            ],
            source: "

                #define IONENGINE_MATRIX_M objects[instance].model
                #define IONENGINE_MATRIX_V world.view
                #define IONENGINE_MATRIX_P world.projection
                #define IONENGINE_MATRIX_INV_M objects[instance].inverse_model

                vs_output main(vs_input input, uint instance : SV_InstanceID) {
                    vs_output output;

                    float4x4 model_view = mul(IONENGINE_MATRIX_M, IONENGINE_MATRIX_V);
                    model_view[0][0] = 1;
                    model_view[0][1] = 0;
                    model_view[0][2] = 0;
                    model_view[1][0] = 0;
                    model_view[1][1] = 1;
                    model_view[1][2] = 0;
                    model_view[2][0] = 0;
                    model_view[2][1] = 0;
                    model_view[2][2] = 1;

                    float4 view_position = mul(float4(input.position, 1.0f) * float4(0.15f, 0.15f, 1.0, 1.0), model_view);

                    output.position = mul(view_position, IONENGINE_MATRIX_P);
                    output.uv = float2(input.uv.x, 1.0 - input.uv.y);
                    return output;
                }
            "
        },
        {
            type: "pixel",
            inputs: [
                { name: "position", type: "f32x4", semantic: "SV_POSITION" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "swapchain", type: "f32x4", semantic: "SV_TARGET0" }
            ],
            source: "   
                ps_output main(ps_input input) {
                    ps_output output;
                    output.swapchain = color_texture.Sample(color_sampler, input.uv);
                    return output;
                }
            "
        }
    ]
}
