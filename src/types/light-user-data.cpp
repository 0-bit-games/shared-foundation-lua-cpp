//
// light-user-data.cpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#include "./light-user-data.hpp"

using namespace foundation::lua::types;

LuaLightUserData::~LuaLightUserData() { }

LuaType::Kind LuaLightUserData::kind() const {
	return LuaType::Kind::lightUserData;
}

void* LuaLightUserData::value() const {
	return lua_touserdata(this->state(), (int)this->stackIndex());
}

LuaLightUserData::LuaLightUserData(
	State& state
) : LuaType(
		state) { }
