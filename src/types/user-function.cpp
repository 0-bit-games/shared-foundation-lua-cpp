//
// user-function.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#include "../exceptions/runtime-exception.hpp"

#include "./user-function.hpp"

using namespace fart::lua::types;
using namespace fart::lua::exceptions;

LuaUserFunction::~LuaUserFunction() { }

int LuaUserFunction::callback(
	lua_State* L
) {

	State& state = *(State*)lua_touserdata(L, lua_upvalueindex(1));
	int64_t index = lua_tointeger(L, lua_upvalueindex(2));

	Array<LuaType> arguments;

	try {

		while (state._available() > 0) {
			arguments.append(
				LuaType::_pick(
					state));
		}

		state._withAutoPopped([&](const ::function<void(const LuaType&)> autoPop) {

			autoPop(state.fart(
				((LuaUserFunction*)state._stack[index]->value)
					->_call(
						arguments
							.map<Type>([](const LuaType& value) {
								return value.fart();
							}))));

		});


		return 1;

	} catch (const RuntimeException& exception) {
		exception.message()
			.withCString([&](const char* message) {
				lua_pushstring(
					state,
					message);
			});
	} catch (const Exception& exception) {
		lua_pushstring(
			state,
			exception.description());
	} catch (...) {
		lua_pushstring(
			state,
			"Unknown exception occurred.");
	}

	lua_error(
		state);

	return 0;

}

LuaUserFunction::LuaUserFunction(
	State& state,
	const ::function<Strong<Type>(const Array<Type>& arguments)> function
) : LuaFunction(state),
	_function(function) { }

Strong<Type> LuaUserFunction::_call(
	const Array<>& arguments
) const {
	return this->_function(
		arguments);
}
