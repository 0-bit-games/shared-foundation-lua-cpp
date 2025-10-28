//
// table.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/10
// See license in LICENSE.
//

#include "../lua/lua-5.4.8/include/lua.hpp"

#include "../exceptions/exceptions.hpp"

#include "./boolean.hpp"
#include "./number.hpp"

#include "./table.hpp"

using namespace foundation::lua::types;
using namespace foundation::lua::exceptions;

LuaTable::~LuaTable() { }

LuaType::Kind LuaTable::kind() const {
	return LuaType::Kind::table;}

Strong<Type> LuaTable::value() noexcept(false) {

	Array<Pair<LuaType, Type>> crossReferences;

	return this->_value(
		crossReferences);

}

Strong<LuaType> LuaTable::get(
	LuaString& key
) const {
	return this->_get(key);
}

Strong<LuaType> LuaTable::get(
	const String& key
) const {
	return this->get(
		this->state().string(key));
}

Strong<LuaType> LuaTable::get(
	LuaNumber& key
) const {
	return this->_get(key);
}

Strong<LuaType> LuaTable::get(
	int64_t key
) const {
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

	Array<Pair<LuaType, Type>> crossReference;

	this->_set(
		key,
		value,
		crossReference);

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

	Array<Pair<LuaType, Type>> crossReference;

	this->_set(
		key,
		value,
		crossReference);

}

void LuaTable::rawSet(
	LuaType& key,
	LuaType& value
) {

	this->_state
		->_withAutoPopped(
			[&](const ::function<void(const LuaType&)> autoPop) {

				autoPop(key.push());
				autoPop(value.push());

				lua_rawset(
					this->state(),
					(int)this->stackIndex());

			});

}

Strong<LuaTable> LuaTable::getMetaTable() {

	auto table = this->push();

	lua_getmetatable(
		this->state(),
		(int)table->stackIndex());

	return LuaType::_pick(
		this->state())
		.as<LuaTable>();

}

void LuaTable::setMetaTable(
	LuaTable& metaTable
) {

	auto table = this->push();

	this->_state
		->_withAutoPopped(
			[&](const ::function<void(const LuaType&)> autoPop) {

				autoPop(metaTable.push());

				lua_setmetatable(
					this->state(),
					(int)table->stackIndex());

			});

}

void LuaTable::resetMetaTable() {

	auto table = this->push();

	this->_state
		->_withAutoPopped(
			[&](const ::function<void(const LuaType&)> autoPop) {

				autoPop(this->state().nil());

				lua_setmetatable(
					this->state(),
					(int)table->stackIndex());

			});

}

int64_t LuaTable::count() {

	lua_len(
		this->state(),
		(int)this->stackIndex());

	Strong<LuaType> result = LuaType::_pick(
		this->state());

	if (result->kind() != LuaType::Kind::number) {
		return 0;
	}

	return result
		.as<LuaNumber>()
		->value();

}

void LuaTable::forEach(
	std::function<void(LuaType& key, LuaType& value)> todo
) noexcept(false) {

	auto table = this->push();

	Strong<LuaType> key = this->state().nil()
		.as<LuaType>();

	this->_state
		->_withAutoPopped(
			[&](const ::function<void(const LuaType&)> autoPop) {

				key = key->push();

				autoPop(key);

				while (lua_next(table->state(), (int)table->stackIndex())) {

					Strong<LuaType> value = LuaType::_pick(
						this->state());

					key = key->replaced();

					todo(key, value);

				}

			});

}

LuaTable::LuaTable(
	State& state
) : LuaType(
		state) { }

Strong<Type> LuaTable::_value(
	Array<Pair<LuaType, Type>>& crossReferences
) noexcept(false) {

	for (size_t idx = 0 ; idx < crossReferences.count() ; idx++) {
		if (crossReferences[idx]->first().rawEquals(*this)) {
			return crossReferences[idx]->second()
				.as<Type>();
		}
	}

	Strong<Dictionary<Type>> result;

	Array<Pair<LuaType, Type>> crossReferencesNested = crossReferences
		.appending(
			Strong<Pair<LuaType, Type>>(
				*this,
				result
					.as<Type>()));

	this->forEach(
		[&](LuaType& key, LuaType& value) {
			result->set(
				key.foundation(),
				value
					._foundation(
						false,
						crossReferencesNested));
		});

	if (result->keys()->every([](const Type& key, size_t idx) {
		return key.kind() == Type::Kind::number && Number<size_t>::getValue(key) == idx + 1;
	})) return result->values()
		.as<Type>();

	return result
		.as<Type>();

}

void LuaTable::_set(
	const String& key,
	const Type& value,
	Array<Pair<LuaType, Type>>& crossReferences
) noexcept(false) {

	this->set(
		this->state().string(key),
		this->state()
			._foundation(
				value,
				crossReferences));

}

void LuaTable::_set(
	int64_t key,
	const Type& value,
	Array<Pair<LuaType, Type>>& crossReferences
) noexcept(false) {

	this->set(
		this->state().number(key),
		this->state()
			._foundation(
				value,
				crossReferences));

}

Strong<LuaType> LuaTable::_get(
	LuaType& key
) const {

	this->_state
		->_withAutoPopped(
			[&](const ::function<void(const LuaType&)> autoPop) {

				autoPop(key.push());

				lua_gettable(
					this->state(),
					(int)this->stackIndex());

			});

	return LuaType::_pick(
		this->state());

}

void LuaTable::_set(
	LuaType& key,
	LuaType& value
) {

	this->_state
		->_withAutoPopped(
			[&](const ::function<void(const LuaType&)> autoPop) {

				autoPop(key.push());
				autoPop(value.push());

				lua_settable(
					this->state(),
					(int)this->stackIndex());

			});

}
