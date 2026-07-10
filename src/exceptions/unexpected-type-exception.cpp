//
// unexpected-type-exception.cpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2023/11/12
// See license in LICENSE.
//

#include "./unexpected-type-exception.hpp"

using namespace foundation::lua::exceptions;
using namespace foundation::lua::types;

UnexpectedTypeException::UnexpectedTypeException(
	LuaType::Kind expectedKind,
	LuaType::Kind encounteredKind
) : _expectedKind(expectedKind), _encounteredKind(encounteredKind) { }

UnexpectedTypeException::~UnexpectedTypeException() { }

const char* UnexpectedTypeException::description() const {
	return "Unexpected type was encountered.";
}

Exception* UnexpectedTypeException::clone() const {
	return new UnexpectedTypeException(
		this->_expectedKind,
		this->_encounteredKind);
}

LuaType::Kind UnexpectedTypeException::expectedKind() const {
	return this->_expectedKind;
}

LuaType::Kind UnexpectedTypeException::encounteredKind() const {
	return this->_encounteredKind;
}
