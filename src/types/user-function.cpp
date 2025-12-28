//
// user-function.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#include "../exceptions/runtime-exception.hpp"

#include "./user-function.hpp"

using namespace foundation::lua::types;
using namespace foundation::lua::exceptions;

LuaUserFunction::~LuaUserFunction() { }

int LuaUserFunction::callback(
	lua_State* L
) {

	foundation::lua::State* state = *((State**)lua_getextraspace(L));

	UserFunctionCallback callback = (UserFunctionCallback)lua_touserdata(L, lua_upvalueindex(1));
	void* context = lua_touserdata(L, lua_upvalueindex(2));

	Strong<String> errorMessage = nullptr;

	try {
		return state
			->_withStackPointer<int>(
				0,
				[&]() {

					Array<LuaType> arguments;

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

							return (int)result->count();

						});

				});
	} catch (const RuntimeException& exception) {
		errorMessage = exception.message();
	} catch (const Exception& exception) {
		errorMessage = Strong<String>(exception.description());
	} catch (...) {
		errorMessage = Strong<String>("An unknown error occurred.");
	}

	if (!errorMessage.equals(nullptr)) {

		errorMessage
			->withCString([&](const char* message) {
				lua_pushstring(L, message);
			});

		errorMessage = nullptr;

		return lua_error(
			*state);

	}

	return 0;

}

LuaUserFunction::LuaUserFunction(
	State& state
) : LuaFunction(state) { }
