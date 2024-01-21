//
// user-function.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#ifndef fart_lua_user_function_hpp
#define fart_lua_user_function_hpp

#include "./function.hpp"

namespace fart::lua::types {

	class LuaUserFunction : public LuaFunction {

		friend class State;
		friend class Strong<LuaUserFunction>;

		public:

			static int callback(lua_State* L);

			virtual ~LuaUserFunction();

		private:

			LuaUserFunction(
				State& state,
				const ::function<Strong<Array<LuaType>>(const Array<LuaType>&)> function);

			Strong<Array<LuaType>> _call(
				const Array<LuaType>& argument
			) const;

			const ::function<Strong<Array<LuaType>>(const Array<LuaType>& arguments)> _function;

	};

}

#endif /* fart_lua_user_function_hpp */
