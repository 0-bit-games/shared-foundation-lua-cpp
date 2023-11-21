//
// table.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/10
// See license in LICENSE.
//

#include <lua.hpp>

#include "../exceptions/exceptions.hpp"

#include "./boolean.hpp"
#include "./number.hpp"
#include "./stab.h"

#include "./table.hpp"

using namespace fart::lua::types;
using namespace fart::lua::exceptions;

LuaTable::~LuaTable() { }

LuaType::Kind LuaTable::kind() const {
	return LuaType::Kind::table;}

Strong<Type> LuaTable::value() noexcept(false) {

	Strong<Dictionary<Type>> result;

	auto nil = this->state().nil();

	this->_restack();

	nil->_restack(true);

	Strong<LuaType> key = nullptr;

	while (lua_next(this->state(), -2)) {

		key = LuaType::_pick(
			this->state(),
			-2);

		auto value = LuaType::_pick(
			this->state());

		result->set(
			key->fart(),
			value->fart());

		key->autoReplaced();

	}

	this->state()._flushAutoReplaced();

	if (result->keys()->every([](const Type& key, size_t idx) {
		return key.kind() == Type::Kind::number && Number<size_t>::getValue(key) == idx + 1;
	})) return result->values()
		.as<Type>();

	return result
		.as<Type>();

}

Strong<LuaType> LuaTable::get(
	LuaString& key
) {
	return this->_get(key);
}

Strong<LuaType> LuaTable::get(
	const String& key
) {
	return this->get(
		this->state().string(key));
}

Strong<LuaType> LuaTable::get(
	LuaNumber& key
) {
	return this->_get(key);
}

Strong<LuaType> LuaTable::get(
	int64_t key
) {
	return this->get(
		this->state().number(key));
}

void LuaTable::set(
	LuaString& key,
	LuaType& value
) {
	this->_set(key, value);
}

void LuaTable::set(
	const String& key,
	const Type& value
) noexcept(false) {
	this->set(
		this->state().string(key),
		this->state().fart(value));
}

void LuaTable::set(
	LuaNumber& key,
	LuaType& value
) {
	this->_set(key, value);
}

void LuaTable::set(
	int64_t key,
	const Type& value
) noexcept(false) {
	this->set(
		this->state().number(key),
		this->state().fart(value));
}

size_t LuaTable::count() {
	return luaL_len(this->state(), this->stackIndex());
}

LuaTable::LuaTable(
	State& state
) : LuaType(
		state) { }

Strong<LuaType> LuaTable::_get(
	LuaType& key
) {

	key._restack(true);

	lua_gettable(this->state(), this->stackIndex());

	return LuaType::_pick(
		this->state());

}

void LuaTable::_set(
	LuaType& key,
	LuaType& value
) {

	key._restack(true);
	value._restack(true);

	lua_settable(this->state(), this->stackIndex());

	this->state()._flushAutoReplaced();

}
