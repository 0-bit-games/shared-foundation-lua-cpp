//
// compiler-exception.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/14
// See license in LICENSE.
//

#include <string.h>

#include "./compiler-exception.hpp"

using namespace foundation::lua::exceptions;

#if defined(_WIN32)
#define strduplicate _strdup
#else
#define strduplicate strdup
#endif

CompilerException::CompilerException() :
	_message(nullptr) { }

CompilerException::CompilerException(
	const char* message
) : _message(strduplicate(message)) { }

CompilerException::~CompilerException() {
	if (this->_message != nullptr) {
		free((void*)_message);
	}
}

const char* CompilerException::description() const {
	return this->_message == nullptr ? "Compilation failed." : this->_message;
}

Exception* CompilerException::clone() const {
	return new CompilerException();
}
