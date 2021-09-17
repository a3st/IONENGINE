// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#ifndef ENUM_CLASS_BIT_FLAG_DECLARE
#define ENUM_CLASS_BIT_FLAG_DECLARE(EnumClass) \
EnumClass operator|(const EnumClass lhs, const EnumClass rhs) { \
	return static_cast<EnumClass>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs)); \
} \
bool operator&(const EnumClass lhs, const EnumClass rhs) { \
	return static_cast<uint32>(lhs) & static_cast<uint32>(rhs); \
}
#endif // ENUM_CLASS_BIT_FLAG_DECLARE

#ifndef STD_TIE_CMP_OPERATOR_DECLARE
#define STD_TIE_CMP_OPERATOR_DECLARE(Struct) \
bool operator==(const Struct& lhs, const Struct& rhs) { \
	return lhs.make_tie() == rhs.make_tie(); \
} \
bool operator<(const Struct& lhs, const Struct& rhs) { \
	return lhs.make_tie() < rhs.make_tie(); \
} \
bool operator>(const Struct& lhs, const Struct& rhs) { \
	return lhs.make_tie() > rhs.make_tie(); \
} \
bool operator!=(const Struct& lhs, const Struct& rhs) { \
	return lhs.make_tie() != rhs.make_tie(); \
}
#endif // STD_TIE_CMP_OPERATOR_DECLARE