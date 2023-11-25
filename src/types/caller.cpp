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
	LuaTable& value
) {
	this->_arguments.append(value);
	return *this;
}

Caller& Caller::argument(
	const Type& value
) {
	this->_arguments.append(
		this->_function.state().fart(value));
	return *this;
}

Strong<LuaType> Caller::exec() {

	bool success = this->_function
		.state()
		._withAutoPopped<bool>(
			[&](const function<void(const LuaType&)> autoPop) {

				auto fnc = this->_function.push();

				autoPop(fnc);

				this->_arguments
					.forEach([&](LuaType& argument) {
						autoPop(argument.push());
					});

				return this->_function.state()
					._withStackPointer<bool>(
						-(this->_arguments.count()),
						[&]() {
							return lua_pcall(this->_function.state(), this->_arguments.count(), 1, 0) == LUA_OK;
						});

			});

	if (!success) {
		String message = lua_tostring(this->_function.state(), -1);
		lua_pop(this->_function.state(), 1);
		throw RuntimeException(message);
	}

	Array<LuaType> result;

	while (this->_function.state()._available()) {
		result
			.append(
				LuaType::_pick(
					this->_function.state()));
	}

	return result.first();

}
