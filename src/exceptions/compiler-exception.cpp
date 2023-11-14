//
// compiler-exception.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/14
// See license in LICENSE.
//

#include "./compiler-exception.hpp"

using namespace fart::lua::exceptions;

CompilerException::CompilerException() { }

CompilerException::~CompilerException() { }

const char* CompilerException::description() const {
	return "Compilation failed.";
}

Exception* CompilerException::clone() const {
	return new CompilerException();
}
