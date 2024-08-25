//
// number.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include "../lua/lua-5.4.7/include/lua.hpp"

#include "../state.hpp"

#include "./number.hpp"

using namespace fart::lua::types;

LuaNumber::~LuaNumber() { }

LuaType::Kind LuaNumber::kind() const {
	return LuaType::Kind::number;
}

bool LuaNumber::isInteger() const {
	return lua_isinteger(this->state(), (int)this->stackIndex());
}

double LuaNumber::value() const {
	return lua_tonumber(this->state(), (int)this->stackIndex());
}

LuaNumber::LuaNumber(
	State& state
) : LuaType(
		state) { }
