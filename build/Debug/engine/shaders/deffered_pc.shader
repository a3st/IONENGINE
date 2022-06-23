{
    name: "deffered_pc",
    conditions: [],
    uniforms: [
        {
            name: "world",
            type: "cbuffer",
            data: [
                { name: "view", type: "f32x4x4" },
                { name: "proj", type: "f32x4x4" },
                { name: "camera_position", type: "f32x3" },
            ],
            visibility: "vertex"
        },
        {
            name: "albedo",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "normal",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "roughness",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "metalness",
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
        cull_mode: "back",
        depth_test: "less",
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
                { name: "world_position", type: "f32x3", semantic: "POSITION" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" },
                { name: "normal", type: "f32x3", semantic: "NORMAL" },
                { name: "tangent", type: "f32x3", semantic: "TANGENT" },
                { name: "bitangent", type: "f32x3", semantic: "BITANGENT" }
            ],
            source: "

                #define IONENGINE_MATRIX_M objects[instance].model
                #define IONENGINE_MATRIX_V world.view
                #define IONENGINE_MATRIX_P world.proj
                #define IONENGINE_MATRIX_INV_M objects[instance].inverse_model

                vs_output main(vs_input input, uint32 instance : SV_InstanceID) {
                    f32x4 world_pos = mul(IONENGINE_MATRIX_M, f32x4(input.position, 1.0f));

                    vs_output output;
                    
                    // vertex in world space
                    output.world_position = world_pos.xyz;

                    // vertex in screen space
                    output.position = mul(IONENGINE_MATRIX_P, mul(IONENGINE_MATRIX_V, world_pos));
                    
                    output.uv = f32x2(input.uv.x, 1.0f - input.uv.y);

                    output.normal = mul((float3x3)transpose(IONENGINE_MATRIX_INV_M), input.normal);
                    output.tangent = mul((float3x3)IONENGINE_MATRIX_M, input.tangent);
                    output.bitangent = mul((float3x3)IONENGINE_MATRIX_M, input.bitangent);

                    return output;
                }
            "
        },
        {
            type: "pixel",
            inputs: [
                { name: "position", type: "f32x4", semantic: "SV_POSITION" },
                { name: "world_position", type: "f32x3", semantic: "POSITION" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" },
                { name: "normal", type: "f32x3", semantic: "NORMAL" },
                { name: "tangent", type: "f32x3", semantic: "TANGENT" },
                { name: "bitangent", type: "f32x3", semantic: "BITANGENT" }
            ],
            outputs: [
                { name: "world_position", type: "f32x4", semantic: "SV_TARGET0" },
                { name: "albedo", type: "f32x4", semantic: "SV_TARGET1" },
                { name: "world_normal", type: "f32x4", semantic: "SV_TARGET2" },
                { name: "roughness_metalness_ao", type: "f32x4", semantic: "SV_TARGET3" },
            ],
            source: "

                ps_output pack_gbuffer_data(f32x3 positions, f32x3 albedo, f32x3 normals, f32 roughness, f32 metalness, f32 ao) {
                    ps_output output;
                    output.world_position = f32x4(positions, 1.0f);
                    output.albedo = f32x4(albedo, 1.0f);
                    output.world_normal = f32x4(normals, 1.0f);
                    output.roughness_metalness_ao = f32x4(roughness, metalness, ao, 1.0f);
                    return output;
                }

                ps_output main(ps_input input) {

                    f32x3 normal = normalize(input.normal);
                    f32x3 tangent = normalize(input.tangent);
                    f32x3 bitangent = normalize(input.bitangent);
                    f32x3x3 tbn = f32x3x3(tangent, bitangent, normal);

                    f32x3 normal_map = TEXTURE_SAMPLE(normal, input.uv).xyz * 2.0 - f32x3(1.0f, 1.0f, 1.0f);
                    normal = normalize(mul(normal_map, tbn));

                    f32x3 albedo = TEXTURE_SAMPLE(albedo, input.uv).rgb;
                    f32 roughness = TEXTURE_SAMPLE(roughness, input.uv).r; 
                    f32 metalness = TEXTURE_SAMPLE(metalness, input.uv).r;
                    f32 ao = 0.0f;

                    return pack_gbuffer_data(
                        input.world_position,
                        albedo,
                        normal,
                        roughness,
                        metalness,
                        ao
                    );
                }
            "
        }
    ]
}
