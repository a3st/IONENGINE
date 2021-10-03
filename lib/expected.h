// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class T, class E>
class Expected {
public:

    Expected(const std::variant<T, E>& value) : m_value(value) {

    }

    const std::variant<T, E>& get() const { return m_value; }

    T value() {

        if(auto result = std::get_if<T>(&m_value)) {
            return *result;
        } 
        auto result = std::get_if<E>(&m_value);
        throw std::runtime_error(*result);
    }

private:

    std::variant<T, E> m_value;
};

template<class T, class E>
inline Expected<T, E> make_expected(const std::variant<T, E>& value) {
    
    return Expected<T, E>(value);
}

template<class T, class E>
void expected_result(
    const Expected<T, E>& expected,
    const std::function<void(const T&)> ok_func, 
    const std::function<void(const E&)> err_func
) {

    if(auto result = std::get_if<T>(&expected.get())) {
        ok_func(*result);
    } else if(auto result = std::get_if<E>(&expected.get())) {
        err_func(*result);
    }
}

}