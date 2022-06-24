{
    name: "ssr_pc",
    conditions: [],
    uniforms: [
        {
            name: "world",
            type: "cbuffer",
            data: [
                { name: "view", type: "f32x4x4" },
                { name: "proj", type: "f32x4x4" },
                { name: "camera_position", type: "f32x3" },
                { name: "inverse_view_proj", type: "f32x4x4" }
            ],
            visibility: "pixel"
        },
        {
            name: "color",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "positions",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "normals",
            type: "sampler2D",
            visibility: "pixel"
        },
        {
            name: "roughness_metalness_ao",
            type: "sampler2D",
            visibility: "pixel"
        }
    ],
    draw_parameters: {
        fill_mode: "solid",
        cull_mode: "back",
        depth_test: "none",
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
                { name: "position", type: "f32x4", semantic: "SV_POSITION" },
                { name: "uv", type: "f32x2", semantic: "TEXCOORD0" }
            ],
            outputs: [
                { name: "color", type: "f32x4", semantic: "SV_TARGET0" }
            ],
            source: "

                #define IONENGINE_MATRIX_V world.view
                #define IONENGINE_MATRIX_P world.proj

                #define SSR_RAY_STEPS 16
                #define SSR_BINARY_STEPS 16

                float2 get_uv_position(float3 position) {
                    float4 view_position = mul(IONENGINE_MATRIX_P, mul(IONENGINE_MATRIX_V, float4(position, 1.0f)));
                    view_position.xy /= view_position.w;
                    view_position.xy = view_position.xy * float2(0.5f, 0.5f) + float2(0.5f, -0.5f);
                    return view_position.xy;
                }

                float4 binary_search(float3 direction, inout float3 hit_position) {
                    float distance;

                    for (uint i = 0; i < SSR_BINARY_STEPS; ++i) {
                        float2 v_proj_coords = get_uv_position(hit_position);

                        float3 reflect_position = positions_texture.Sample(positions_sampler, v_proj_coords).xyz;
                        distance = length(hit_position - reflect_position);

                        if (distance <= 1.0f) {
                            hit_position += direction;
                        }
                        
                        direction *= 0.5f;
                        hit_position -= direction;
                    }

                    float2 v_proj_coords = get_uv_position(hit_position);

                    float3 reflect_position = positions_texture.Sample(positions_sampler, v_proj_coords).xyz;
                    distance = length(hit_position - reflect_position);

                    return float4(v_proj_coords, distance, abs(distance) < 2.0f ? 1.0f : 0.0f);
                }

                float4 ray_march(float3 direction, inout float3 hit_position) {
                    float2 v_proj_coords;

                    for (uint i = 0; i < SSR_RAY_STEPS; ++i) {
                        hit_position += direction;
                        
                        v_proj_coords = get_uv_position(hit_position);

                        float3 reflect_position = positions_texture.Sample(positions_sampler, v_proj_coords).xyz;
                        float distance = length(hit_position - reflect_position);

                        if(distance > 0.0f) {
                            return binary_search(direction, hit_position);
                        }

                        direction *= 1.6f;
                    }
                    return float4(0.0f, 0.0f, 0.0f, 0.0f);
                }

                ps_output main(ps_input input) {
                    ps_output output;

                    float3 color = color_texture.Sample(color_sampler, input.uv).rgb;

                    float3 roughness_metalness_ao = roughness_metalness_ao_texture.Sample(roughness_metalness_ao_sampler, input.uv).rgb;
                    float metalness = roughness_metalness_ao.g;

                    if(metalness < 0.01f) {
                        output.color = float4(color, 1.0f);
                        return output;
                    }

                    float3 position = positions_texture.Sample(positions_sampler, input.uv).xyz;
                    float3 normal = normals_texture.Sample(normals_sampler, input.uv).xyz;

                    float3 V = normalize(world.camera_position - position);

                    float3 reflect_dir = normalize(reflect(V, normal));
                    float3 hit_position = position;

                    float4 v_coords = ray_march(reflect_dir, hit_position);

                    float2 v_coords_edge_fact = float2(1.0f, 1.0f) - pow(saturate(abs(v_coords.xy - float2(0.5f, 0.5f)) * 2), 8);
                    float screen_edge_factor = saturate(min(v_coords_edge_fact.x, v_coords_edge_fact.y));

                    float reflection_intensity = saturate(
                        screen_edge_factor * // screen fade
                        saturate(reflect_dir.z) * // camera facing fade
                        v_coords.w
                    );

                    float3 reflection_color = reflection_intensity * color_texture.Sample(color_sampler, v_coords.xy, 0).rgb;

                    output.color = float4(color, 1.0f) + metalness * max(0.0f, float4(reflection_color, 1.0f));
                    return output;
                }
            "
        }
    ]
}
