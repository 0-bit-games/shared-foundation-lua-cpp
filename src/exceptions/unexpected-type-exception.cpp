//
// unexpected-type-exception.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include "./unexpected-type-exception.hpp"

using namespace fart::lua::exceptions;
using namespace fart::lua::types;

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
