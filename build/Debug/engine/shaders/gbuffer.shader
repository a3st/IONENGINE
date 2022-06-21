{
    name: "gbuffer_pc",
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
            visibility: "all"
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
            properties: [
                { name: "model", type: "float4x4" },
                { name: "inverse_model", type: "float4x4" }
            ],
            visibility: "vertex"
        }
    ],
    draw_parameters: {
        fill_mode: "solid",
        cull_mode: "back",
        depth_stencil: true,
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
                { name: "world_position", type: "float3", semantic: "POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" },
                { name: "normal", type: "float3", semantic: "NORMAL" },
                { name: "tangent", type: "float3", semantic: "TANGENT" },
                { name: "bitangent", type: "float3", semantic: "BITANGENT" }
            ],
            source: "

                #define IONENGINE_MATRIX_M objects[instance].model
                #define IONENGINE_MATRIX_V world.view
                #define IONENGINE_MATRIX_P world.proj
                #define IONENGINE_MATRIX_INV_M objects[instance].inverse_model

                vs_output main(vs_input input, uint instance : SV_InstanceID) {
                    vs_output output;

                    float4 world_pos = mul(IONENGINE_MATRIX_M, float4(input.position, 1.0f));
                    
                    // vertex in world space
                    output.world_position = world_pos.xyz;

                    // vertex in screen space
                    output.position = mul(IONENGINE_MATRIX_P, mul(IONENGINE_MATRIX_V, world_pos));
                    
                    output.uv = float2(input.uv.x, 1.0f - input.uv.y);

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
                { name: "position", type: "float4", semantic: "SV_POSITION" },
                { name: "world_position", type: "float3", semantic: "POSITION" },
                { name: "uv", type: "float2", semantic: "TEXCOORD0" },
                { name: "normal", type: "float3", semantic: "NORMAL" },
                { name: "tangent", type: "float3", semantic: "TANGENT" },
                { name: "bitangent", type: "float3", semantic: "BITANGENT" }
            ],
            outputs: [
                { name: "positions", type: "float4", semantic: "SV_TARGET0" },
                { name: "albedo", type: "float4", semantic: "SV_TARGET1" },
                { name: "normals", type: "float4", semantic: "SV_TARGET2" },
                { name: "roughness_metalness_ao", type: "float4", semantic: "SV_TARGET3" },
            ],
            source: "

                ps_output pack_gbuffer(float3 positions, float3 albedo, float3 normal, float roughness, float metalness) {
                    ps_output output;
                    output.positions = float4(positions, 1.0f);
                    output.albedo = float4(albedo, 1.0f);
                    output.normals = float4(normal, 1.0f);
                    output.roughness_metalness_ao = float4(roughness, metalness, 0.0f, 1.0f);
                    return output;
                }

                ps_output main(ps_input input) {

                    float3 normal = normalize(input.normal);
                    float3 tangent = normalize(input.tangent);
                    float3 bitangent = normalize(input.bitangent);
                    float3x3 tbn = float3x3(tangent, bitangent, normal);
                    float3 normal_map = normal_texture.Sample(normal_sampler, input.uv).xyz * 2.0 - float3(1.0f, 1.0f, 1.0f);

                    float3 albedo = albedo_texture.Sample(albedo_sampler, input.uv).rgb;
                    float roughness = roughness_texture.Sample(roughness_sampler, input.uv).r;
                    float metalness = metalness_texture.Sample(metalness_sampler, input.uv).r;

                    return pack_gbuffer(
                        input.world_position,
                        albedo,
                        normalize(mul(normal_map, tbn)),
                        roughness,
                        metalness
                    );
                }
            "
        }
    ]
}
