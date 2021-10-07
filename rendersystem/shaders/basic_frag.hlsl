// Copyright © 2019-2021 Dmitriy Lukovenko. All rights reserved.

struct PSInput {

	float4 position : SV_POSITION;
};

float4 main(PSInput input) : SV_TARGET {
    
	return float4(input.x, input.y, input.z, 1.0f);
}
