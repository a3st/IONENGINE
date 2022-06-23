{
    name: "forward_pc",
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
            visibility: "all"
        },
        {
            name: "light",
            type: "cbuffer",
            data: [
                { name: "point_light_count", type: "uint32" }
            ],
            visibility: "all"
        },
        {
            name: "albedo",
            type: "sampler2D",
            visibility: "pixel",
        },
        {
            name: "normal",
            type: "sampler2D",
            visibility: "pixel",
        },
        {
            name: "roughness",
            type: "sampler2D",
            visibility: "pixel",
        },
        {
            name: "metalness",
            type: "sampler2D",
            visibility: "pixel",
        },
        {
            name: "point_light",
            type: "sbuffer",
            data: [
                { name: "position", type: "f32x3" },
                { name: "range", type: "f32" },
                { name: "color", type: "f32x3" }
            ],
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
        blend_mode: "alpha_blend"
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
                { name: "color", type: "f32x4", semantic: "SV_TARGET0" }
            ],
            source: "

                #define DIRECTIONAL_LIGHT_TYPE 0
                #define POINT_LIGHT_TYPE 1
                #define SPOT_LIGHT_TYPE 2

                static const f32 PI = 3.14159265359;

                f32 distribution_ggx(f32x3 N, f32x3 H, f32 roughness) {
                    f32 a = roughness * roughness;
                    f32 a2 = a * a;
                    f32 NdotH = max(dot(N, H), 0.0f);
                    f32 NdotH2 = NdotH * NdotH;

                    f32 num = a2;
                    f32 denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
                    denom = PI * denom * denom;

                    return num / max(denom, 0.0000001);
                }

                f32 geometry_schlick_ggx(f32 NdotV, f32 roughness) {
                    f32 r = (roughness + 1.0f);
                    f32 k = (r * r) / 8.0f;

                    f32 num = NdotV;
                    f32 denom = NdotV * (1.0f - k) + k;

                    return num / denom;
                }

                f32 geometry_smith(f32x3 N, f32x3 V, f32x3 L, f32 roughness) {
                    f32 NdotV = max(dot(N, V), 0.0f);
                    f32 NdotL = max(dot(N, L), 0.0f);
                    f32 ggx2 = geometry_schlick_ggx(NdotV, roughness);
                    f32 ggx1 = geometry_schlick_ggx(NdotL, roughness);

                    return ggx1 * ggx2;
                }

                f32x3 fresnel_schlick(f32 cos_theta, f32x3 F0) {
                    return F0 + (1.0 - F0) * pow(max(1.0f - cos_theta, 0.0f), 5.0f);
                }

                f32 calculate_attenuation(f32 distance, f32 range) {
                    f32 att = saturate(1.0f - (distance * distance / (range * range)));
                    return att * att;
                }

                f32x3 calculate_point_light(point_light_data point_light, f32x3 position, f32x3 normal, f32x3 V, f32x3 albedo, f32 metalness, f32 roughness) {

                    f32x3 F0 = float3(0.04f, 0.04f, 0.04f);
                    F0 = lerp(F0, albedo, metalness);

                    f32x3 L = normalize(position - point_light.position);
                    f32x3 H = normalize(V + L);

                    // Calculate per-light radiance
                    f32 distance = length(point_light.position - position);
                    f32 attenuation = calculate_attenuation(distance, point_light.range);
                    f32x3 radiance = point_light.color * attenuation;

                    // Cook-Torrance BRDF
                    f32 NDF = distribution_ggx(normal, H, roughness);        
                    f32 G = geometry_smith(normal, V, L, roughness);      
                    f32x3 F = fresnel_schlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

                    f32x3 nominator = NDF * G * F;
                    f32 denominator = 4.0f * max(dot(normal, V), 0.0f) * max(dot(normal, L), 0.0f) + 0.0001f;
                    f32x3 specular = nominator / denominator;

                    // kS is equal to Fresnel
                    f32x3 kS = F;
                    f32x3 kD = f32x3(1.0f, 1.0f, 1.0f) - kS;
                    kD *= 1.0 - metalness;

                    // Scale light by NdotL
                    f32 NdotL = max(dot(normal, L), 0.0f);

                    // Add to outgoing radiance Lo
                    f32x3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
                    return Lo;
                }

                f32x3 calculate_normals(f32x3 normal, f32x3 tangent, f32x3 bitangent, f32x2 uv) {
                    f32x3x3 tbn = f32x3x3(normalize(tangent), normalize(bitangent), normalize(normal));
                    f32x3 normal_map = TEXTURE_SAMPLE(normal, uv).xyz * 2.0 - float3(1.0f, 1.0f, 1.0f);
                    return normalize(mul(normal_map, tbn));
                }

                ps_output main(ps_input input) {
                    f32x3 normal = calculate_normals(input.normal, input.tangent, input.bitangent, input.uv);
                    f32x3 albedo = TEXTURE_SAMPLE(albedo, input.uv).rgb;
                    f32 roughness = TEXTURE_SAMPLE(roughness, input.uv).r;
                    f32 metalness = TEXTURE_SAMPLE(metalness, input.uv).r;
                    f32 opacity = TEXTURE_SAMPLE(albedo, input.uv).a;

                    f32x3 V = normalize(input.world_position - world.camera_position);

                    f32x3 Lo = float3(0.0f, 0.0f, 0.0f);

                    for(uint32 i = 0; i < light.point_light_count; ++i) {
                        Lo += calculate_point_light(point_lights[i], input.world_position, normal, V, albedo, metalness, roughness);
                    }

                    f32x3 color = pow(Lo, f32x3(1.4f / 2.2f, 1.4f / 2.2f, 1.4f / 2.2f));

                    ps_output output;
                    output.color = f32x4(color, opacity);
                    return output;
                }
            "
        }
    ]
}
