// Copyright © 2019-2021 Dmitriy Lukovenko. All rights reserved.

struct VSInput {
	float3 position : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

cbuffer WorldBuffer : register(b0) {
	float4x4 model;
	float4x4 view;
	float4x4 proj;
};

struct VSOutput {
	float4 position : SV_POSITION;
};

VSOutput main(VSInput input) {
    
	VSOutput output;

	float4 newPosition = float4(input.position, 1.0f);

	newPosition = mul(newPosition, model);
	newPosition = mul(newPosition, view);
	newPosition = mul(newPosition, proj);

	output.position = newPosition;

	return output;
}