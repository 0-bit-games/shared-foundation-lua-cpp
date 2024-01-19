//
// global.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/11
// See license in LICENSE.
//

#include "../build/lua/src/lua.hpp"

#include "./global.hpp"

using namespace fart::lua;

Global::~Global() { }

Global::Global(
	State& state
) : LuaTable(state) { }
