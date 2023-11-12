//
// boolean.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include <lua.hpp>

#include "../state.hpp"

#include "./boolean.hpp"

using namespace fart::lua::types;

LuaBoolean::~LuaBoolean() { }

LuaType::Kind LuaBoolean::kind() const {
	return LuaType::Kind::boolean;
}

bool LuaBoolean::value() const {
	return lua_toboolean(this->state(), this->stackIndex());
}

LuaBoolean::LuaBoolean(
	State& state
) : LuaType(state) { }
