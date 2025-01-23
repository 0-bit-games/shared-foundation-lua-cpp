//
// runtime-exception.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/06
// See license in LICENSE.
//

#include "./runtime-exception.hpp"

using namespace foundation::lua::exceptions;

RuntimeException::RuntimeException(
	const String& message
) : _message(message) { }

RuntimeException::~RuntimeException() { }

const char* RuntimeException::description() const {
	return "Lua raised an error.";
}

const String& RuntimeException::message() const {
	return this->_message;
}

Exception* RuntimeException::clone() const {
	return new RuntimeException(this->_message);
}
