// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

enum class MouseButton {
	Left,
	Right,
	Middle,
	Four,
	Five
};

enum class ElementState {
	Pressed,
	Released
};

enum class WindowEvent {
	Closed,
	Sized,
	Moved,
	KeyboardInput,
	MouseInput,
	MouseWheel,
	MouseMoved
};

template<typename T>
struct PhysicalSize {
	T width, height;

	PhysicalSize() : width(0), height(0) {
    }

	PhysicalSize(const T width_, const T height_) : width(width_), height(height_) {
    }
};


template<typename T>
struct PhysicalPosition {
	T x, y;

	PhysicalPosition() : x(0), y(0) {
    }

	PhysicalPosition(const T x_, const T y_) : x(x_), y(y_) {
    }
};

struct KeyboardInput {
	uint32 scan_code;
	ElementState state;

	KeyboardInput() : scan_code(0), state(ElementState::Released) {
    }

	KeyboardInput(const uint32 scan_code_, const ElementState state_) : scan_code(scan_code_), state(state_) {
    }
};

struct MouseInput {
	MouseButton button;
	ElementState state;

	MouseInput() : button(MouseButton::Left), state(ElementState::Released) {
    }

	MouseInput(const MouseButton button_, const ElementState state_) : button(button_), state(state_) {
    }
};

struct MouseWheel {
	float delta;

	MouseWheel() : delta(0.0f) {
    }

	MouseWheel(const float delta_) : delta(delta_) {
    }
};

struct MouseMoved {
	PhysicalPosition<float> position;
	bool relative;

	MouseMoved() : position(0.0f, 0.0f), relative(false) {
    }

	MouseMoved(const float x, const float y, const bool relative_) : position(x, y), relative(relative_) {
    }
};

struct WindowEventHandler {
	uint64_t window_id;
	WindowEvent event_type;

	std::variant<
		PhysicalSize<uint32>,
		PhysicalPosition<uint32>,
		KeyboardInput,
		MouseInput,
		MouseWheel,
		MouseMoved
	> event;
};

}