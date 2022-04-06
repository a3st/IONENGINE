// Copyright © 2019-2022 Dmitriy Lukovenko. All rights reserved.

struct PSInput {
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PSOutput {
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 metalic : SV_TARGET2;
    float4 roughness : SV_TARGET3;
    float4 ao : SV_TARGET4;
};

PSOutput main(PSInput input) {
    
    PSOutput output;

	output.albedo = float4(0.2f, 0.1f, 0.5f, 1.0f);
    output.normal = float4(0.2f, 0.4f, 0.5f, 1.0f);
    output.metalic = float4(0.6f, 0.4f, 0.5f, 1.0f);
    output.roughness = float4(0.2f, 0.4f, 0.5f, 1.0f);
    output.ao = float4(0.2f, 0.4f, 0.6f, 1.0f);

	return output;
}