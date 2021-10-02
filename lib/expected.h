// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

// helper type for the visitor #4
template<class... Ts> struct default_visitor : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> default_visitor(Ts...) -> default_visitor<Ts...>;

template<class T, class E>
class Expected {
public:

    Expected(const std::variant<T, E>& value) : m_value(value) {

    }

    const std::variant<T, E>& get() const { return m_value; }

private:

    std::variant<T, E> m_value;
};

template<class T, class E>
inline Expected<T, E> make_expected(const std::variant<T, E>& value) {
    return Expected<T, E>(value);
}

}