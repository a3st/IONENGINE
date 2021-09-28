// Copyright © 2019-2021 Dmitriy Lukovenko. All rights reserved.

struct PSInput {

	float4 position : SV_POSITION;
};

float4 main(PSInput input) : SV_TARGET {
    
	return float4(1.0f, 0.4f, 1.0f, 1.0f);
}
