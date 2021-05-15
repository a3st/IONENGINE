// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math {

struct Fcolor {
	float r, g, b, a;

	Fcolor() : r(0), g(0), b(0), a(0) { 
	}

	Fcolor(const float r_, const float g_, const float b_, const float a_) : r(r_), g(g_), b(b_), a(a_) {	
	}

	Fcolor(const Fcolor& rhs) : r(rhs.r), g(rhs.g), b(rhs.b), a(rhs.a) {
	}

	Fcolor(Fcolor&& rhs) noexcept : r(rhs.r), g(rhs.g), b(rhs.b), a(rhs.a) {
	}

	Fcolor& operator=(const Fcolor& rhs) {
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	Fcolor& operator=(Fcolor&& rhs) noexcept {
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
		return *this;
	}

	void rgb2srgb() {
		for (int i = 0; i < 3; i++) {
			float color = (&r)[i];
			if (color < 0.0031308f) {
				color = 12.92f * color;
			}
			else {
				color = 1.055f * math::pow(color, 1.0f / 2.4f) - 0.05499995f;
			}
		}
	}

	void srgb2rgb() {
		for (int i = 0; i < 3; i++) {
			float color = (&r)[i];
			if (color <= 0.040448643f) {
				color = color / 12.92f;
			}
			else {
				color = math::pow((color + 0.055f) / 1.055f, 2.4f);
			}
		}
	}

	const float* data() const {
		return &r;
	}

	Fvector4 to_vector4() const {
		return Fvector4(r, g, b, a);
	}

	Fvector3 to_vector3() const {
		return Fvector3(r, g, b);
	}
    
	bool operator==(const Fcolor& rhs) const {
		return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
	}

	bool operator!=(const Fcolor& rhs) const {
		return (r != rhs.r) || (g != rhs.g) || (b != rhs.b) || (a != rhs.a);
	}
};

}