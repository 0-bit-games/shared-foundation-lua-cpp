//
// lua-exception.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/06
// See license in LICENSE.
//

#include "./lua-exception.hpp"

using namespace fart::lua::exceptions;

LuaException::LuaException(
	const String& message
) : _message(message) { }

LuaException::~LuaException() { }

const char* LuaException::description() const {
	return "Lua raised an error.";
}

const String& LuaException::message() const {
	return this->_message;
}

Exception* LuaException::clone() const {
	return new LuaException(this->_message);
}
