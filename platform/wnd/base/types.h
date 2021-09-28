// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform::wnd {

class WindowEventLoop;
class Window;

enum class WindowStyle : uint32 {
	Normal = 1 << 0,
    Minimize = 1 << 1,
    Maximaze = 1 << 2,
	Borderless =
		(uint32)Normal |
		(uint32)Minimize |
		(uint32)Maximaze
};

ENUM_CLASS_BIT_FLAG_DECLARE(WindowStyle)

enum class MouseButton {
	Left,
	Right,
	Middle,
	Four,
	Five
};

enum class ElementState {
	Released,
	Pressed
};

enum class WindowEvent {
	Closed,
	Sized,
	Moved,
	KeyboardInput,
	MouseInput,
	MouseWheel,
	MouseMoved,
	Updated
};

struct PhysicalSize {
	uint32 width;
	uint32 height;
};

struct PhysicalPosition {
	int32 x;
	int32 y;
};

struct KeyboardInput {
	uint32 scan_code;
	ElementState state;
};

struct MouseInput {
	MouseButton button;
	ElementState state;
};

struct MouseWheel {
	float delta;
};

struct MouseMoved {
	PhysicalPosition position;
	bool relative;
};

struct WindowEventHandler {
	uint64 window_id;
	WindowEvent event_type;
	std::variant<
		PhysicalSize,
		PhysicalPosition,
		KeyboardInput,
		MouseInput,
		MouseWheel,
		MouseMoved
	> event;
};

}