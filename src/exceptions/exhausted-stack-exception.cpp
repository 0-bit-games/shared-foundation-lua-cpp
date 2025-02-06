//
// exhausted-exception.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2025/02/06
// See license in LICENSE.
//

#include "../exceptions/exhausted-stack-exception.hpp"

using namespace foundation::lua::exceptions;

ExhaustedStackException::ExhaustedStackException() { }

ExhaustedStackException::~ExhaustedStackException() { }

const char* ExhaustedStackException::description() const {
	return "Stack exhausted.";
}

Exception* ExhaustedStackException::clone() const {
	return new ExhaustedStackException();
}
