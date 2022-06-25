{
    name: "test",
    conditions: [
        "HAS_ALBEDO_TEXTURE",
        "HAS_NORMAL_TEXTURE",
        "HAS_OPACITY_TEXTURE",
        "HAS_ROUGHNESS_TEXTURE",
        "HAS_METALNESS_TEXTURE",
        "HAS_EMISSIVE_TEXTURE"
    ],
    uniforms: [
        {
            name: "world",
            type: "cbuffer",
            properties: [
                { name: "view", type: "float4x4_t" },
                { name: "proj", type: "float4x4_t" },
                { name: "camera_position", type: "float3_t" },
                { name: "inverse_view", type: "float4x4_t" }
            ],
            visibility: "vertex"
        },
        {
            name: "material",
            type: "cbuffer",
            data: [
                { name: "albedo", type: "float3_t" },
                { name: "opacity", type: "float_t" },
                { name: "normal", type: "float3_t" },
                { name: "roughness", type: "float_t" },
                { name: "metalness", type: "float_t" },
                { name: "emissive", type: "float3_t" }
            ],
            visibility: "pixel"
        },
        {
            name: "albedo",
            type: "sampler2D",
            visibility: "pixel",
            if: "HAS_ALBEDO_TEXTURE"
        },
        {
            name: "opacity",
            type: "sampler2D",
            visibility: "pixel",
            if: "HAS_OPACITY_TEXTURE"
        },
        {
            name: "normal",
            type: "sampler2D",
            visibility: "pixel",
            if: "HAS_NORMAL_TEXTURE"
        },
        {
            name: "roughness",
            type: "sampler2D",
            visibility: "pixel",
            if: "HAS_ROUGHNESS_TEXTURE"
        },
        {
            name: "metalness",
            type: "sampler2D",
            visibility: "pixel",
            if: "HAS_METALNESS_TEXTURE"
        },
        {
            name: "emissive",
            type: "sampler2D",
            visibility: "pixel",
            if: "HAS_EMISSIVE_TEXTURE"
        }
    ],
    draw_parameters: {
        fill_mode: "solid",
        cull_mode: "none",
        stencil_test: "none",
        depth_test: "less",
        blend_mode: "alpha_blend"
    },
    stages: [
        {
            type: "vertex",
            inputs: [
                { name: "position", type: "float3_t", semantic: "POSITION" },
                { name: "uv", type: "float2_t", semantic: "TEXCOORD0" },
                { name: "normal", type: "float3_t", semantic: "NORMAL" },
                { name: "tangent", type: "float3_t", semantic: "TANGENT" },
                { name: "bitangent", type: "float3_t", semantic: "BITANGENT" },
                { name: "weights", type: "float4_t", semantic: "BLENDWEIGHT", if: "HAS_SKIN" }
            ],
            outputs: [
                { name: "position", type: "float4_t", semantic: "SV_POSITION" },
                { name: "world_position", type: "float3_t", semantic: "POSITION" },
                { name: "uv", type: "float2_t", semantic: "TEXCOORD0" },
                { name: "normal", type: "float3_t", semantic: "NORMAL" },
                { name: "tangent", type: "float3_t", semantic: "TANGENT" },
                { name: "bitangent", type: "float3_t", semantic: "BITANGENT" },
                { name: "weights", type: "float4_t", semantic: "BLENDWEIGHT", if: "HAS_SKIN" }
            ],
            source: "

                vs_output main(vs_input input) {
                    vs_output output;
                    return output;
                }
            "
        },
        {
            type: "pixel",
            inputs: [
                { name: "position", type: "float4_t", semantic: "SV_POSITION" },
                { name: "world_position", type: "float3_t", semantic: "POSITION" },
                { name: "uv", type: "float2_t", semantic: "TEXCOORD0" },
                { name: "normal", type: "float3_t", semantic: "NORMAL" },
                { name: "tangent", type: "float3_t", semantic: "TANGENT" },
                { name: "bitangent", type: "float3_t", semantic: "BITANGENT" },
                { name: "weights", type: "float4_t", semantic: "BLENDWEIGHT", if: "HAS_SKIN" }
            ],
            outputs: [
                { name: "color", type: "float4_t", semantic: "SV_TARGET0" }
            ],
            source: "

                #ifndef TEXTURE_SAMPLE
                #define TEXTURE_SAMPLE(Texture, UV) Texture##_texture.Sample(Texture##_sampler, UV) 
                #endif

                #ifndef float3_t
                #define float3_t float3
                #endif

                #ifndef float2_t
                #define float2_t float2
                #endif

                #ifndef float_t
                #define float_t float
                #endif

                #ifndef float4x4_t
                #define float4x4_t float4x4
                #endif

                #ifndef uint_t
                #define uint_t uint
                #endif

                #ifndef bool_t
                #define bool_t bool
                #endif

                ps_output main(ps_input input) {

                    #ifdef HAS_ALBEDO_TEXTURE
                    float3_t albedo = material.albedo;
                    #else
                    float3_t albedo = TEXTURE_SAMPLE(albedo, input.uv).rgb;
                    #endif

                    ps_output output = {
                        float4_t(albedo, 1.0f)
                    };

                    return output;
                }
            "
        }
    ]
}
