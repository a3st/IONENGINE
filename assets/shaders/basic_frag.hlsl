// Copyright © 2019-2021 Dmitriy Lukovenko. All rights reserved.

struct PSInput {
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

Texture2D texture_base : register(t0);
SamplerState sampleWrap : register(s0);

float4 main(PSInput input) : SV_TARGET {
    
	float4 diffuse_color = texture_base.Sample(sampleWrap, input.uv);
	return diffuse_color;
}
