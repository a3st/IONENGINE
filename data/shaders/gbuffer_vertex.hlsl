// Copyright © 2019-2022 Dmitriy Lukovenko. All rights reserved.

struct VSInput {
	float3 position : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VSOutput {
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

cbuffer ViewData : register(b0) {
	float4x4 view;
	float4x4 projection;
};

VSOutput main(VSInput input) {

	float4 newPosition = float4(input.position, 1.0f);

    VSOutput output;
    output.position = newPosition;
	output.uv = input.uv;
	output.normal = input.normal;

	return output;
}
