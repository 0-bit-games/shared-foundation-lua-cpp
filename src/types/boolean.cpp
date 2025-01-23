//
// boolean.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include "../lua/lua-5.4.7/include/lua.hpp"

#include "../state.hpp"

#include "./boolean.hpp"

using namespace foundation::lua::types;

LuaBoolean::~LuaBoolean() { }

LuaType::Kind LuaBoolean::kind() const {
	return LuaType::Kind::boolean;
}

bool LuaBoolean::value() const {
	return lua_toboolean(this->state(), (int)this->stackIndex());
}

LuaBoolean::LuaBoolean(
	State& state
) : LuaType(
		state) { }
