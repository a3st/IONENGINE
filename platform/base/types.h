// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

class WindowEventLoop;
class Window;

enum class WindowStyle : uint32;
enum class MessageBoxStyle : uint32;

WindowStyle operator|(const WindowStyle lhs, const WindowStyle rhs) {
	return static_cast<WindowStyle>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

MessageBoxStyle operator|(const MessageBoxStyle lhs, const MessageBoxStyle rhs) {
	return static_cast<MessageBoxStyle>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

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