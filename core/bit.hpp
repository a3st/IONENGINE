// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#ifndef DECLARE_ENUM_CLASS_BIT_FLAG
#define DECLARE_ENUM_CLASS_BIT_FLAG(EnumClass)                                                                         \
    using EnumClass##Flags = uint32_t;                                                                                 \
    inline auto operator|(EnumClass const lhs, EnumClass const rhs)->EnumClass##Flags                                  \
    {                                                                                                                  \
        return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);                                                \
    }                                                                                                                  \
    inline auto operator|(EnumClass##Flags const lhs, EnumClass const rhs)->EnumClass##Flags                           \
    {                                                                                                                  \
        return lhs | static_cast<uint32_t>(rhs);                                                                       \
    }                                                                                                                  \
    inline auto operator&(EnumClass const lhs, EnumClass const rhs)->bool                                              \
    {                                                                                                                  \
        return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);                                                \
    }                                                                                                                  \
    inline auto operator&(EnumClass##Flags const lhs, EnumClass const rhs)->bool                                       \
    {                                                                                                                  \
        return lhs & static_cast<uint32_t>(rhs);                                                                       \
    }
#endif // DECLARE_ENUM_CLASS_BIT_FLAG