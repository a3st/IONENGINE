// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

std::wstring stws(const std::string& src) {
    size_t length = ::strlen(src.c_str()) + 1;
    assert(src.length() > 0 && "src source length is less than 0 or equal 0");
    size_t out_size = 0;
    std::wstring out_str(length - 1, 0);
    ::mbstowcs_s(&out_size, &out_str[0], length, src.c_str(), length - 1);
    return out_str;
}

std::string wsts(const std::wstring& src) {
    size_t length = ::wcslen(src.c_str()) + 1;
    assert(src.length() > 0 && "src source length is less than 0 or equal 0");
    size_t out_size = 0;
    std::string out_str(length - 1, 0);
    ::wcstombs_s(&out_size, &out_str[0], length, src.c_str(), length - 1);
    return out_str;
}

template<class T, typename... A>
T format(const T& fmt, A&&... args) {
    assert(fmt.length() > 0 && "fmt source length is less than 0 or equal 0");
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