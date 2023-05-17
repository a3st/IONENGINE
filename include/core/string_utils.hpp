// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ie {

namespace core {

auto wstring_convert(std::string_view const source) -> std::wstring {
    int32_t length = MultiByteToWideChar(
        CP_UTF8, 
        0, 
        source.data(), 
        static_cast<int32_t>(source.size()),
        nullptr,
        0
    );

    std::wstring dest(length, '\0');

    MultiByteToWideChar(
        CP_UTF8, 
        0, 
        source.data(), 
        static_cast<int32_t>(source.size()),
        dest.data(),
        length
    );
    return dest;
}

auto string_convert(std::wstring_view const source) -> std::string {
    int32_t length = WideCharToMultiByte(
        CP_UTF8, 
        0, 
        source.data(), 
        static_cast<int32_t>(source.size()), 
        nullptr, 
        0, 
        nullptr, 
        nullptr
    );
    
    std::string dest(length, '\0');

    WideCharToMultiByte(
        CP_UTF8, 
        0, 
        source.data(), 
        static_cast<int32_t>(source.size()), 
        dest.data(), 
        static_cast<int32_t>(dest.size()), 
        nullptr, 
        nullptr
    );
    return dest;                                 
}

}

}