// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

//#include "../lmath/lmath.h"

namespace ionengine::rendersystem {

struct Color {
	float r, g, b, a;

	Color() : r(0), g(0), b(0), a(0) { 
	
    }

	Color(const float _r, const float _g, const float _b, const float _a) : r(_r), g(_g), b(_b), a(_a) {	
	
    }

	Color(const Color& rhs) : r(rhs.r), g(rhs.g), b(rhs.b), a(rhs.a) {
	
    }

	Color(Color&& rhs) noexcept : r(rhs.r), g(rhs.g), b(rhs.b), a(rhs.a) {
	
    }

	Color& operator=(const Color& rhs) {

		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	Color& operator=(Color&& rhs) noexcept {

		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	void RGBToSRGB() {

		for (int32_t i = 0; i < 3; ++i) {
			float color = (&r)[i];
			if (color < 0.0031308f) {
				color = 12.92f * color;
			} else {
				color = 1.055f * std::powf(color, 1.0f / 2.4f) - 0.05499995f;
			}
		}
	}

	void SRGBToRGB() {

		for (int32_t i = 0; i < 3; ++i) {
			float color = (&r)[i];
			if (color <= 0.040448643f) {
				color = color / 12.92f;
			} else {
				color = std::powf((color + 0.055f) / 1.055f, 2.4f);
			}
		}
	}

	const float* GetData() const {

		return &r;
	}

	size_t GetSize() const {

		return 4;
	}

	/*lmath::Vector4f ToVector4() const {

		return lmath::Vector4f { r, g, b, a };
	}

	lmath::Vector3f ToVector3() const {

		return lmath::Vector3f { r, g, b };
	}*/
    
	bool operator==(const Color& rhs) const {

		return std::tie(r, g, b, a) == std::tie(rhs.r, rhs.g, rhs.b, rhs.a);
	}

	bool operator!=(const Color& rhs) const {

		return std::tie(r, g, b, a) != std::tie(rhs.r, rhs.g, rhs.b, rhs.a);
	}
};

}