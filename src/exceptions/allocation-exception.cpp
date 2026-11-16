//
// allocation-exception.cpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2025/11/16
// See license in LICENSE.
//

#include "../exceptions/allocation-exception.hpp"

using namespace foundation::lua::exceptions;

AllocationException::AllocationException() { }

AllocationException::~AllocationException() { }

const char* AllocationException::description() const {
	return "Could not allocate Lua state.";
}

Exception* AllocationException::clone() const {
	return new AllocationException();
}
