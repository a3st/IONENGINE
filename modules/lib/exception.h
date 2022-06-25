// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

class Exception : public std::exception {
public:

    Exception(std::string_view const error_msg) : _error_msg(error_msg) { }

    const char* what() const noexcept {
    	return reinterpret_cast<const char*>(_error_msg.c_str());
    }

private:

    std::string _error_msg;
};

}