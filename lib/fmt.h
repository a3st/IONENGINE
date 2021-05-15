// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<class T, typename... A>
T format(const T& fmt, A&&... args) {
    assert(fmt.size() > 0 && "fmt source length is less than 0 or equal 0");
    return internal::format(fmt, std::forward<A>(args)...);
}

namespace internal {

template<class T, typename I, typename... A>
T format(const T& fmt, const I& arg, A&&... args) {
    auto it_arg = std::find(fmt.begin(), fmt.end(), '{');
    auto it_type = std::next(it_arg);

    if (it_arg == fmt.end() || it_type == fmt.end()) {
        return format(fmt);
    }

    using char_type_t = typename T::value_type; 
    std::basic_stringstream<char_type_t, std::char_traits<char_type_t>, std::allocator<char_type_t>> ss;
    ss << T(fmt.begin(), it_arg);

    switch (*it_type) {
        case '}': {
            ss << arg << T(it_type + 1, fmt.end());
            break;
        }
        case 'x': {
            if constexpr (std::is_integral<I>::value) {
                auto it_optional = std::next(it_type);
                if (*it_optional == ':') {
                    ++it_optional;
                    if (*it_optional != '}') {
                        ss <<
                            std::hex << 
                            std::setw(static_cast<std::streamsize>(*it_optional - 48)) << 
                            std::setfill('0') << static_cast<unsigned>(arg) << 
                            std::dec;

                        if (*std::next(it_optional) != '}') {
                            ss << T(it_optional + 3, fmt.end());
                        } else {
                            ss << T(it_optional + 2, fmt.end());
                        }
                    }
                }
            } else {
                ss << arg << T(it_type + 1, fmt.end());
            }
            break;
        }
    }
    return internal::format(ss.str(), std::forward<A>(args)...);
}

template<class T>
T format(const T& fmt) {
    return fmt;
}

}
}