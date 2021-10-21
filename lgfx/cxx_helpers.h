// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#ifndef DECLARE_ENUM_CLASS_BIT_FLAG
#define DECLARE_ENUM_CLASS_BIT_FLAG(EnumClass) \
inline EnumClass operator|(const EnumClass lhs, const EnumClass rhs) { \
	return static_cast<EnumClass>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)); \
} \
inline bool operator&(const EnumClass lhs, const EnumClass rhs) { \
	return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs); \
}
#endif // DECLARE_ENUM_CLASS_BIT_FLAG

#ifndef DECLARE_STRUCT_OPERATOR_COMPARE
#define DECLARE_STRUCT_OPERATOR_COMPARE(Struct) \
inline bool operator==(const Struct& lhs, const Struct& rhs) { \
	return lhs.make_tie() == rhs.make_tie(); \
} \
inline bool operator<(const Struct& lhs, const Struct& rhs) { \
	return lhs.make_tie() < rhs.make_tie(); \
} \
inline bool operator>(const Struct& lhs, const Struct& rhs) { \
	return lhs.make_tie() > rhs.make_tie(); \
} \
inline bool operator!=(const Struct& lhs, const Struct& rhs) { \
	return lhs.make_tie() != rhs.make_tie(); \
}
#endif // DECLARE_STRUCT_OPERATOR_COMPARE