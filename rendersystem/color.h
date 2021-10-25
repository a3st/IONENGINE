// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

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

	inline const float* GetData() const {

		return &r;
	}

	inline size_t GetSize() const {

		return 4;
	}

	void SRGBToRGB();
	void RGBToSRGB();
    
	inline bool operator==(const Color& rhs) const {

		return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
	}

	inline bool operator!=(const Color& rhs) const {

		return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a;
	}
};

}