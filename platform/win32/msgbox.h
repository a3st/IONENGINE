// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

enum class MessageBoxStyle : uint32 {
    ButtonOK = MB_OK,
    IconError = MB_ICONERROR,
    IconWarning = MB_ICONINFORMATION
};

MessageBoxStyle operator|(const MessageBoxStyle lhs, const MessageBoxStyle rhs) {
	return static_cast<MessageBoxStyle>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

int32 show_message_box(void* hwnd, const std::wstring& msg, const std::wstring& label, MessageBoxStyle msgbox_style) {
    return MessageBox(reinterpret_cast<HWND>(hwnd), msg.c_str(), label.c_str(), static_cast<uint32>(msgbox_style));
}

}