//
// not-supported-exception.cpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2023/11/12
// See license in LICENSE.
//

#include "./not-supported-exception.hpp"

using namespace foundation::lua::exceptions;

NotSupportedException::NotSupportedException() { }

NotSupportedException::~NotSupportedException() { }

const char* NotSupportedException::description() const {
	return "Feature is not supported in Lua.";
}

Exception* NotSupportedException::clone() const {
	return new NotSupportedException();
}
