// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include "color.h"

using namespace ionengine::renderer;

void Color::RGBToSRGB() {

	for (int32_t i = 0; i < 3; ++i) {
		float color = (&r)[i];
		if (color < 0.0031308f) {
			color = 12.92f * color;
		} else {
			color = 1.055f * std::powf(color, 1.0f / 2.4f) - 0.05499995f;
		}
	}
}

void Color::SRGBToRGB() {

	for (int32_t i = 0; i < 3; ++i) {
		float color = (&r)[i];
		if (color <= 0.040448643f) {
			color = color / 12.92f;
		} else {
			color = std::powf((color + 0.055f) / 1.055f, 2.4f);
		}
	}
}