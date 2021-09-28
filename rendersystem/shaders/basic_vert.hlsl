// Copyright © 2019-2021 Dmitriy Lukovenko. All rights reserved.

struct VSInput {

	float3 position : POSITION;
};

struct VSOutput {

	float4 position : SV_POSITION;
};

VSOutput main(VSInput input) {
    
	VSOutput output;

	float4 newPosition = float4(input.position, 1.0f);

//	newPosition = mul(newPosition, model);
//	newPosition = mul(newPosition, view);
//	newPosition = mul(newPosition, projection);

	output.position = newPosition;

	return output;
}