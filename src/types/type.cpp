//
// type.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include "../lua/lua-5.4.7/include/lua.hpp"

#include "./type.hpp"
#include "../exceptions/exceptions.hpp"
#include "../state.hpp"

#include "./boolean.hpp"
#include "./number.hpp"
#include "./string.hpp"
#include "./function.hpp"
#include "./table.hpp"
#include "./value.hpp"

using namespace fart::lua::types;
using namespace fart::lua::exceptions;

template<typename T>
Strong<T> _to(const LuaType* value, LuaType::Kind kind) noexcept(false) {

	if (value->kind() != LuaType::Kind::nil && value->kind() != kind) {
		throw UnexpectedTypeException(
			kind,
			value->kind());
	}

	return Strong<T>((T*)value);

}

LuaType::~LuaType() {
	this->_state->_popStackItem(this);
}

LuaType::Kind LuaType::kind() const {
	return LuaType::Kind::nil;
}

String LuaType::kindDescription() const {
	switch (this->kind()) {
		case LuaType::Kind::nil: return "nil";
		case LuaType::Kind::boolean: return "boolean";
		case LuaType::Kind::number: return "number";
		case LuaType::Kind::string: return "string";
		case LuaType::Kind::function: return "function";
		case LuaType::Kind::table: return "table";
		case LuaType::Kind::lightUserData: return "lightUserData";
		default: break;
	}
	throw NotSupportedException();
}

Strong<Type> LuaType::fart(
	bool trueNull
) const noexcept(false) {
	switch (this->kind()) {
		case LuaType::Kind::nil:
			if (trueNull) return nullptr;
			return Strong<Null>()
				.as<Type>();
		case LuaType::Kind::boolean:
			return Strong<Boolean>(
				((LuaBoolean&)*this).value())
				.as<Type>();
		case LuaType::Kind::number:
			return Strong<Float>(
				((LuaNumber&)*this).value())
				.as<Type>();
		case LuaType::Kind::string:
			return Strong<String>(
				((LuaString&)*this).value())
				.as<Type>();
		case LuaType::Kind::table:
			return Strong<Type>(
				((LuaTable&)*this).value()
				.as<Type>());
		default:
			break;
	}
	throw NotSupportedException();
}

Strong<LuaBoolean> LuaType::boolean() const noexcept(false) {
	return _to<LuaBoolean>(this, Kind::boolean);
}

Strong<LuaNumber> LuaType::number() const noexcept(false) {
	return _to<LuaNumber>(this, Kind::number);
}

Strong<LuaString> LuaType::string() const noexcept(false) {
	return _to<LuaString>(this, Kind::string);
}

Strong<LuaFunction> LuaType::function() const noexcept(false) {
	return _to<LuaFunction>(this, Kind::function);
}

Strong<LuaTable> LuaType::table() const noexcept(false) {
	return _to<LuaTable>(this, Kind::table);
}

Strong<LuaLightUserData> LuaType::lightUserData() const noexcept(false) {
	return _to<LuaLightUserData>(this, Kind::lightUserData);
}

bool LuaType::operator==(
	nullptr_t
) const {
	return this->kind() == Kind::nil;
}

bool LuaType::operator!=(
	nullptr_t
) const {
	return !(this->operator==(nullptr));
}

LuaType::LuaType(
	State& state
) : _state(state),
	_stackOffset(0) { }

ssize_t LuaType::stackIndex() const {
	return this->stackIndexOf(
		this->_stackOffset);
}

ssize_t LuaType::stackIndexOf(
	size_t stackOffset
) const {
	return (ssize_t)stackOffset - (ssize_t)this->state()._stackPointer();
}

Strong<LuaType> LuaType::underlying() const {
	return Strong<LuaType>(
		(LuaType&)*this);
}

Strong<LuaType> LuaType::push() const {
	lua_pushvalue(*this->_state, (int)this->stackIndex());
	return this->_state->_pushStackItem<LuaValue>(
		*this)
		.as<LuaType>();
}

Strong<LuaType> LuaType::replaced() {
	switch (lua_type(this->state(), (int)this->stackIndex())) {
		case LUA_TNIL: return this->state()._replaceStackItem<LuaType>(
			this->_stackOffset);
		case LUA_TBOOLEAN: return this->state()._replaceStackItem<LuaBoolean>(
			this->_stackOffset)
			.as<LuaType>();
		case LUA_TNUMBER: return this->state()._replaceStackItem<LuaNumber>(
			this->_stackOffset)
			.as<LuaType>();
		case LUA_TSTRING: return this->state()._replaceStackItem<LuaString>(
			this->_stackOffset)
			.as<LuaType>();
		case LUA_TFUNCTION: return this->state()._replaceStackItem<LuaFunction>(
			this->_stackOffset)
			.as<LuaType>();
		case LUA_TTABLE: return this->state()._replaceStackItem<LuaTable>(
			this->_stackOffset)
			.as<LuaType>();
		case LUA_TLIGHTUSERDATA: return this->state()._replaceStackItem<LuaLightUserData>(
			this->_stackOffset)
			.as<LuaType>();
		default:
			throw NotSupportedException();
	}
}

Strong<LuaType> LuaType::_pick(
	State& state,
	ssize_t offset
) {
	switch (lua_type(state, (int)(state._nextIndex() + offset))) {
		case LUA_TNIL: return state._pushStackItem<LuaType>();
		case LUA_TBOOLEAN: return state._pushStackItem<LuaBoolean>()
			.as<LuaType>();
		case LUA_TNUMBER: return state._pushStackItem<LuaNumber>()
			.as<LuaType>();
		case LUA_TSTRING: return state._pushStackItem<LuaString>()
			.as<LuaType>();
		case LUA_TFUNCTION: return state._pushStackItem<LuaFunction>()
			.as<LuaType>();
		case LUA_TTABLE: return state._pushStackItem<LuaTable>()
			.as<LuaType>();
		case LUA_TLIGHTUSERDATA: return state._pushStackItem<LuaLightUserData>()
			.as<LuaType>();
		default:
			throw NotSupportedException();
	}
}
