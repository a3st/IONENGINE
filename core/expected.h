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

    friend Expected make_expected<Type, Error>(Type&& element);
    
    friend Expected make_expected<Type, Error>(Error&& element);

public:

    Expected(Expected const&) = default;

    Expected(Expected&&) noexcept = default;

    Expected& operator=(Expected const&) = default;

    Expected& operator=(Expected&&) noexcept = default;

    bool is_ok() const {
        return _data.index() == 0;
    }

    bool is_error() const {
        return _data.index() == 1;
    }

    Type as_ok() {
        return std::move(std::get<0>(_data));
    }

    Error as_error() {
        return std::move(std::get<1>(_data));
    }

private:

    Expected(std::variant<Type, Error>&& element) : _data(std::move(element)) { }

    std::variant<Type, Error> _data;
};

template<class Type, class Error>
inline Expected<Type, Error> make_expected(Type&& element) {
    return Expected<Type, Error>(std::move(element));
}

template<class Type, class Error>
inline Expected<Type, Error> make_expected(Error&& element) {
    return Expected<Type, Error>(std::move(element));
}

}
