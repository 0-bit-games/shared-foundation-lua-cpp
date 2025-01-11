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

	fart::lua::State* state = *((State**)lua_getextraspace(L));

	UserFunctionCallback callback = (UserFunctionCallback)lua_touserdata(L, lua_upvalueindex(1));
	void* context = lua_touserdata(L, lua_upvalueindex(2));

	return state
		->_withStackPointer<int>(
			0,
			[&]() {

				Array<LuaType> arguments;

				try {

					while (state->_available() > 0) {
						arguments.append(
							LuaType::_pick(
								*state));
					}

					return state
						->_withAutoPopped<int>([&](const ::function<void(const LuaType&)> autoPop) {

							auto result = callback(
								*state,
								context,
								arguments);

							result
								->forEach([&](const LuaType& result) {
									autoPop(result);
								});

							return result->count();

						});

				} catch (const RuntimeException& exception) {
					exception.message()
						.withCString([&](const char* message) {
							lua_pushstring(
								*state,
								message);
						});
				} catch (const Exception& exception) {
					lua_pushstring(
						*state,
						exception.description());
				} catch (...) {
					lua_pushstring(
						*state,
						"Unknown exception occurred.");
				}

				lua_error(
					*state);

				return 0;

			});

}

LuaUserFunction::LuaUserFunction(
	State& state
) : LuaFunction(state) { }
