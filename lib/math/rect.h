// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math {

template<typename T>
struct Rect {

    T left, top, right, bottom;

	Rect() : left(0), top(0), right(0), bottom(0) {
    
    }

	Rect(const T left_, const T top_, const T right_, const T bottom_) : left(left_), top(top_), right(right_), bottom(bottom_) {
    
    }

	Rect(const Rect& rhs) : left(rhs.left), top(rhs.top), right(rhs.right), bottom(rhs.bottom) {
    
    }

	Rect(Rect&& rhs) noexcept : left(rhs.left), top(rhs.top), right(rhs.right), bottom(rhs.bottom) { 
    
    }

	Rect& operator=(const Rect& rhs) {
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
	    return *this;
    }

	Rect& operator=(Rect&& rhs) noexcept {
	    left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
	    return *this;
    }
};

using Irect = Rect<int32>;
using Frect = Rect<float>;

}