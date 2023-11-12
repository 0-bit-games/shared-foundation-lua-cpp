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

LuaType::LuaType(
	State& state
) : _state(state),
	_stackOffset(state._pushStackItem(
		this)) { }

ssize_t LuaType::stackIndex() const {
	return (ssize_t)this->_stackOffset - ((ssize_t)this->state()._stack.length());
}

void LuaType::autoReplaced() {
	this->_state->_markAutoReplaced(this);
}

Strong<LuaType> LuaType::_pick(
	State& state
) {
	switch (lua_type(state, -1)) {
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
		default:
			throw NotSupportedException();
	}
}

void LuaType::_restack(
	bool autoReplaced
) {
	lua_pushvalue(*this->_state, this->stackIndex());
	this->_state->_pushStackItem(
		autoReplaced ? nullptr : this,
		autoReplaced);
}
