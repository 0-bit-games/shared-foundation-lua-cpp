//
// global.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/11
// See license in LICENSE.
//

#include <lua.hpp>

#include "./global.hpp"

using namespace fart::lua;

Global::~Global() { }

Global::Global(
	State& state
) : LuaTable(state) {
	lua_getglobal(state, "_G");
}
