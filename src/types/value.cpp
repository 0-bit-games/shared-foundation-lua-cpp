//
// value.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#include "./value.hpp"

using namespace fart::lua::types;

LuaValue::~LuaValue() { }

String LuaValue::kindDescription() const {
	return this->underlying()->kindDescription()
		.mapCString<String>([&](const char* kindDescription) {
			return String::format("%s (%zu)", kindDescription, this->_underlying->_stackOffset);
		});
}

Strong<LuaType> LuaValue::underlying() const {
	return this->_underlying->underlying();
}

LuaValue::LuaValue(
	State& state,
	const LuaType& underlying
) : LuaType(state),
	_underlying(underlying) { }
