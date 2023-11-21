//
// function.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include <lua.hpp>

#include "../state.hpp"
#include "../exceptions/exceptions.hpp"
#include "./caller.hpp"

#include "./function.hpp"

using namespace fart::lua::types;
using namespace fart::lua::exceptions;

LuaFunction::~LuaFunction() { }

LuaType::Kind LuaFunction::kind() const {
	return LuaType::Kind::function;
}

Caller LuaFunction::call() {
	return Caller(*this);
}

LuaFunction::LuaFunction(
	State& state
) : LuaType(
		state) { }
