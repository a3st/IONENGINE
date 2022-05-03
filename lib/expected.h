// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class ErrorCode>
struct Result {
    ErrorCode errc;
    std::string message;
};

template<class Type, class Error>
class Expected {
public:

    Expected(Expected const&) = default;

    Expected(Expected&&) noexcept = default;

    Expected& operator=(Expected const&) = default;

    Expected& operator=(Expected&&) noexcept = default;

    static Expected ok(Type&& element) {
        return Expected(std::move(element));
    }

    static Expected error(Error const& element) {
        return Expected(element);
    }

    bool is_ok() const {
        return _data.index() == 0;
    }

    bool is_error() const {
        return _data.index() == 1;
    }

    Type value() {
        return std::move(std::get<0>(_data));
    }

private:

    Expected(std::variant<Type, Error>&& element) : _data(std::move(element)) { }

    std::variant<Type, Error> _data;
};

}
