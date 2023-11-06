//
// lua.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/06
// See license in LICENSE.
//

#include "./lua.hpp"

using namespace fart::lua;
using namespace fart::lua::exceptions;

Lua::Function::Function(
	Strong<Lua> lua,
	const String& name
) : _lua(lua.as<Object>()),
	_name(name),
	_arguments() { }

void Lua::Function::call() const noexcept(false) {
	this->_call(0);
}

Lua::Function& Lua::Function::_argument(
	Strong <Type> value
) {
	this->_arguments
		.append(value);
		return *this;
}

void Lua::Function::_call(
	size_t numberOfResults
) const noexcept(false) {

	this->_name
		.withCString([&](const char* name) {
			lua_getglobal(this->lua(), name);
		});

	this->_arguments
		.forEach([&](const Type& value) {
			switch (value.kind()) {
				case Type::Kind::null:
					lua_pushnil(this->lua());
				case Type::Kind::string:
					value
						.as<String>()
						.withCString([&](const char* value) {
							lua_pushstring(this->lua(), value);
						});
					break;
				case Type::Kind::number:
					switch (value.as<Numeric>().subType()) {
						case Numeric::Subtype::boolean:
							lua_pushboolean(this->lua(), value.as<Boolean>().value());
							break;
						case Numeric::Subtype::floatingPoint:
							lua_pushnumber(this->lua(), value.as<Float>().value());
							break;
						case Numeric::Subtype::integer:
							lua_pushinteger(this->lua(), value.as<Integer>().value());
							break;
					}
					break;
				default:
					throw LuaException("Type is not supported.");
			}
		});

	if (lua_pcall(this->lua(), this->_arguments.count(), numberOfResults, 0) != LUA_OK) {
		throw LuaException(
			this->lua()
				._pop<String>([&]() {
					return lua_tostring(this->lua(), -1);
				}));
	}

}

Strong<Lua> Lua::fromFile(
	const String& filename
) {
	return Strong<Lua>()
		.with([&](Lua& lua) {
			filename
				.withCString([&](const char* file) {
					luaL_dofile(lua, file);
				});
		});
}

Strong<Lua> Lua::fromCode(
	const String& code
) {
	return Strong<Lua>()
		.with([&](Lua& lua) {
			code
				.withCString([&](const char* code) {
					luaL_dostring(lua, code);
				});
		});
}

Lua::~Lua() {
	lua_close(*this);
}

Lua::Function Lua::fnc(
	const String& name
) const {
	return Function(
		Strong<Lua>(this),
		name);
}

Lua::Lua(
) : _L(luaL_newstate()) {
	luaL_openlibs(*this);
}
