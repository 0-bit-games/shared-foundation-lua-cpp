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
	return LuaType::Kind::table;
}

Strong<Array<>> LuaTable::array() noexcept(false) {

	Strong<Array<>> result;

	for (size_t idx = 0 ; idx < this->count() ; idx++) {

		auto value = this->get((int64_t)idx + 1);

		switch (value->kind()) {
			case LuaType::Kind::boolean:
				result->append(Strong<Boolean>(value->boolean()->value())
					.as<Type>());
				break;
			case LuaType::Kind::number:
				if (value->number()->isInteger()) {
					result->append(Strong<Integer>(value->number()->value())
						.as<Type>());
				} else {
					result->append(Strong<Float>(value->number()->value())
						.as<Type>());
				}
				break;
			case LuaType::Kind::string:
				result->append(value->string()->value()
					.as<Type>());
				break;
			case LuaType::Kind::table:
				result->append(value->table()->dictionary()
					.as<Type>());
				break;
			default:
				throw NotSupportedException();
		}

	}

	return result;

}

Strong<Dictionary<Type>> LuaTable::dictionary() noexcept(false) {

	Strong<Dictionary<Type>> result;

	auto nil = this->state().nil();

	nil->_restack(true);

	Strong<LuaType> key = nullptr;

	ssize_t tableStackIndex = this->stackIndex();

	while (lua_next(this->state(), tableStackIndex)) {

		key = LuaType::_pick(
			this->state());

		auto value = LuaType::_pick(
			this->state());

		result->set(
			key->fart(),
			value->fart());

		key->autoReplaced();

	}

	this->state()._flushAutoReplaced();

	return result;

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
) : LuaType(state) { }

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
