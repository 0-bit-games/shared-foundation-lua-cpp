//
// compiler-exception.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/14
// See license in LICENSE.
//

#include <string.h>

#include "./compiler-exception.hpp"

using namespace fart::lua::exceptions;

CompilerException::CompilerException() :
	_message(nullptr) { }

CompilerException::CompilerException(
	const char* message
) : _message(strdup(message)) { }

CompilerException::~CompilerException() {
	if (this->_message != nullptr) {
		free((void*)_message);
	}
}

const char* CompilerException::description() const {
	return this->_message ?: "Compilation failed.";
}

Exception* CompilerException::clone() const {
	return new CompilerException();
}
