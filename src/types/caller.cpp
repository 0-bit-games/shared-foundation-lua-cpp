//
// caller.cpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/10
// See license in LICENSE.
//

#include <time.h>

#include "./function.hpp"
#include "../exceptions/exceptions.hpp"

#include "./caller.hpp"

using namespace fart::lua::types;
using namespace fart::lua::exceptions;

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

Strong<Array<LuaType>> Caller::exec(
	double timeout
) const noexcept(false) {

	bool success = this->_function
		.state()
		._withAutoPopped<bool>(
			[&](const function<void(const LuaType&)> autoPop) {

				auto fnc = this->_function.push();

				autoPop(fnc);

				this->_function.state()
					._withAutoPopped(
						[&](const function<void(const LuaType&)> autoPop) {

							this->_arguments
								.forEach([&](LuaType& argument) {
									autoPop(argument.push());
								});

						});

				return this->_function.state()
					._withStackPointer<bool>(
						0,
						[&]() {

							if (timeout > 0.0) {

								this->_function.state()._currentCallStartTime = time(nullptr);

								lua_sethook(
									this->_function.state(),
									Caller::_executionHook,
									LUA_MASKCOUNT,
									1000);

							}

							return lua_pcall(fnc->state(), (int)this->_arguments.count(), 1, 0) == LUA_OK;

						});

			});

	if (timeout > 0.0) {
		lua_sethook(this->_function.state(), nullptr, 0, 0);
	}

	if (!success) {
		String message = lua_tostring(this->_function.state(), -1);
		lua_pop(this->_function.state(), 1);
		throw RuntimeException(message);
	}

	return LuaType::_pickUnclaimed(
		this->_function.state());

}

Caller::Caller(
	LuaFunction& function
) : _function(function), _arguments() { }

void Caller::_executionHook(
	lua_State* L,
	lua_Debug*
) {

	State* state = *(State **)lua_getextraspace(L);

	time_t currentTime = time(nullptr);

	if (difftime(currentTime, state->_currentCallStartTime) > 0.0) {
		lua_sethook(*state, nullptr, 0, 0);
		luaL_error(*state, "engine.errors.code.executionTimeout");
	}

}
