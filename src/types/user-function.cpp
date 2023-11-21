//
// user-function.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#include "./user-function.hpp"

using namespace fart::lua::types;

LuaUserFunction::~LuaUserFunction() { }

int LuaUserFunction::callback(
	lua_State* L
) {

	State& state = *(State*)lua_touserdata(L, lua_upvalueindex(1));
	int64_t index = lua_tointeger(L, lua_upvalueindex(2));

	int64_t top = lua_gettop(state);

	Array<> arguments;

	for (int64_t idx = 0 ; idx < lua_gettop(state) ; idx++) {

		auto value = LuaType::_pick(
			state,
			idx + 1);

		value->autoReplaced();

		arguments.append(
			value->fart());

	}

	try {

		auto result = state.fart(
			((LuaUserFunction*)state._stack[index]->value)
				->_call(
					arguments));

		result->autoReplaced();

		state._flushAutoReplaced();

		return 1;

	} catch (const Exception& exception) {
		lua_pushstring(
			state,
			exception.description());
	} catch (...) {
		lua_pushstring(
			state,
			"Unknown exception occurred");
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
