//
// allocation-exception.cpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2026/11/15
// See license in LICENSE.
//

#include "allocation-exception.hpp"

using namespace foundation::lua::exceptions;

AllocationException::AllocationException() : foundation::exceptions::Exception() {}

AllocationException::~AllocationException() {}

const char* AllocationException::description() const {
	return "Allocation exception";
}

foundation::exceptions::Exception* AllocationException::clone() const {
	return new AllocationException(*this);
}
