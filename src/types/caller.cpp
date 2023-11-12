//
// caller.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/10
// See license in LICENSE.
//

#include "./function.hpp"
#include "../exceptions/exceptions.hpp"

#include "./caller.hpp"

using namespace fart::lua::types;
using namespace fart::lua::exceptions;

Caller::Caller(
	LuaFunction& function
) : _function(function), _arguments() { }

Caller& Caller::argument(
	LuaBoolean& value
) {
	this->_arguments.append(value);
	return *this;
}

Caller& Caller::argument(
	LuaFunction& value
) {
	this->_arguments.append(value);
	return *this;
}

Caller& Caller::argument(
	LuaNumber& value
) {
	this->_arguments.append(value);
	return *this;
}

Caller& Caller::argument(
	double value
) {
	return this->argument(
		this->_function.state().number(value));
}

Caller& Caller::argument(
	int64_t value
) {
	return this->argument(
		this->_function.state().number(value));
}

Caller& Caller::argument(
	LuaString& value
) {
	this->_arguments.append(value);
	return *this;
}

Caller& Caller::argument(
	const String& value
) {
	return this->argument(
		this->_function.state().string(value));
}

Caller& Caller::argument(
	LuaTable& value
) {
	this->_arguments.append(value);
	return *this;
}

Caller& Caller::argument(
	const Dictionary<Type>& value
) {
	return this->argument(
		this->_function.state().table(value));
}

Caller& Caller::argument(
	const Array<>& value
) {
	return this->argument(
		this->_function.state().table(value));
}

Strong<LuaType> Caller::exec() {

	this->_function._restack(true);

	this->_arguments
		.forEach([](LuaType& argument) {
			argument._restack(true);
		});

	if (lua_pcall(this->_function.state(), this->_arguments.count(), 1, 0) != LUA_OK) {
		String message = lua_tostring(this->_function.state(), -1);
		lua_pop(this->_function.state(), 1);
		throw RuntimeException(message);
	}

	return LuaType::_pick(
		this->_function.state());

}
