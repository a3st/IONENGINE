
struct WorldData {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct LightingData {
    float distance;
    float3 position;
    float3 direction;
    float3 color;
};