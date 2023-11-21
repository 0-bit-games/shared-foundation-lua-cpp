//
// type.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#include <lua.hpp>

#include "./type.hpp"
#include "../exceptions/exceptions.hpp"
#include "../state.hpp"

#include "./boolean.hpp"
#include "./number.hpp"
#include "./string.hpp"
#include "./function.hpp"
#include "./table.hpp"

using namespace fart::lua::types;
using namespace fart::lua::exceptions;

template<typename T>
Strong<T> _to(const LuaType* value, LuaType::Kind kind) noexcept(false) {

	if (value->kind() != kind) {
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
) const noexcept(false) {
	switch (this->kind()) {
		case LuaType::Kind::nil:
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
	_stackOffset(state._pushStackItem(
		this)) { }

ssize_t LuaType::stackIndex() const {
	return this->stackIndexOf(
		this->_stackOffset);
}

ssize_t LuaType::stackIndexOf(
	size_t stackOffset
) const {
	return (ssize_t)this->_stackOffset - ((ssize_t)this->state()._stack.length());
}

void LuaType::autoReplaced() {
	this->_state->_markAutoReplaced(this);
}

Strong<LuaType> LuaType::_pick(
	State& state,
	ssize_t index
) {
	switch (lua_type(state, index)) {
		case LUA_TNIL: return Strong<LuaType>(
			state);
		case LUA_TBOOLEAN: return Strong<LuaBoolean>(
			state)
			.as<LuaType>();
		case LUA_TNUMBER: return Strong<LuaNumber>(
			state)
			.as<LuaType>();
		case LUA_TSTRING: return Strong<LuaString>(
			state)
			.as<LuaType>();
		case LUA_TFUNCTION: return Strong<LuaFunction>(
			state)
			.as<LuaType>();
		case LUA_TTABLE: return Strong<LuaTable>(
			state)
			.as<LuaType>();
		case LUA_TLIGHTUSERDATA: return Strong<LuaLightUserData>(
			state)
			.as<LuaType>();
		default:
			throw NotSupportedException();
	}
}

size_t LuaType::_restack(
	bool autoReplaced
) {
	lua_pushvalue(*this->_state, this->stackIndex());
	return this->_state->_pushStackItem(
		autoReplaced ? nullptr : this,
		autoReplaced);
}
