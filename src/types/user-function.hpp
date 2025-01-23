//
// user-function.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#ifndef foundation_lua_user_function_hpp
#define foundation_lua_user_function_hpp

#include "./function.hpp"

namespace foundation::lua::types {

	class LuaUserFunction : public LuaFunction {

		friend class State;
		friend class Strong<LuaUserFunction>;

		public:

			static int callback(lua_State* L);

			virtual ~LuaUserFunction();

		private:

			LuaUserFunction(
				State& state);

	};

}

#endif /* foundation_lua_user_function_hpp */
