//
// global.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/11
// See license in LICENSE.
//

#include "./lua/lua-5.4.7/include/lua.hpp"

#include "./global.hpp"

using namespace foundation::lua;

Global::~Global() { }

Global::Global(
	State& state
) : LuaTable(state) { }
