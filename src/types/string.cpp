//
// string.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include "../lua/lua-5.4.6/include/lua.hpp"

#include "../state.hpp"

#include "./string.hpp"

using namespace fart::lua::types;

LuaString::~LuaString() { }

LuaType::Kind LuaString::kind() const {
	return LuaType::Kind::string;
}

Strong<String> LuaString::value() const {
	return Strong<String>(
		lua_tostring(this->state(), (int)this->stackIndex()));
}

LuaString::LuaString(
	State& state
) : LuaType(
		state) { }
