//
// global.cpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2023/11/12
// See license in LICENSE.
//

#include "./lua/lua-5.4.8/include/lua.hpp"

#include "./global.hpp"

using namespace foundation::lua;

Global::~Global() { }

Global::Global(
	State& state
) : LuaTable(state) { }
