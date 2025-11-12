//
// caller.cpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/10
// See license in LICENSE.
//

#include <time.h>

#include "./function.hpp"
#include "../exceptions/exceptions.hpp"

#include "./caller.hpp"

using namespace foundation::lua;
using namespace foundation::lua::types;
using namespace foundation::lua::exceptions;

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
		this->_function.state().foundation(value));
	return *this;
}

Strong<Array<LuaType>> Caller::exec() const noexcept(false) {

	bool success = this->_function
		.state()
		._withAutoPopped<bool>(
			[&](const function<void(const LuaType&)> autoPop) {

				Strong<LuaUserFunction> errorHandler = this->_function
					.state()
					.function(
						Caller::_errorHandler,
						(void*)this);

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
							return lua_pcall(
								fnc->state(),
								(int)this->_arguments.count(),
								1,
								errorHandler->stackIndex()
							) == LUA_OK;
						});

			});

	if (!success) {

		String message = lua_tostring(this->_function.state(), -1);
		lua_pop(this->_function.state(), 1);

		if (!this->_errorStackTrace.equals(nullptr)) {
			throw RuntimeException(
				message,
				this->_errorStackTrace);
		}

		throw RuntimeException(
			message,
			{});

	}

	return LuaType::_pickUnclaimed(
		this->_function.state());

}

Strong<Array<DebugInformation>> Caller::errorStackTrace() const noexcept {
	return this->_errorStackTrace;
}

Strong<Array<LuaType>> Caller::_errorHandler(
	State& state,
	void* context,
	const Array<LuaType>& arguments
) {

	Caller* caller = (Caller*)context;

	String message = "(error object is not a string)";

	if (arguments.count() > 0 && arguments[0]->kind() == LuaType::Kind::string) {
		message = arguments[0]
			->foundation(true)
			.as<String>();
	}

	caller->_errorStackTrace = state
		.stackTrace();

	return Strong<Array<LuaType>>(
		state.string(message)
			.as<LuaType>(),
		1);

}

Caller::Caller(
	LuaFunction& function
) : _function(function),
	_arguments(),
	_errorStackTrace(nullptr) { }
