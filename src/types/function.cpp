//
// function.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include "../lua/lua-5.4.8/include/lua.hpp"

#include "../state.hpp"
#include "../exceptions/exceptions.hpp"
#include "./caller.hpp"

#include "./function.hpp"

using namespace foundation::lua::types;
using namespace foundation::lua::exceptions;

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
