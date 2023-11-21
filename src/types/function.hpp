//
// function.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef fart_lua_function_hpp
#define fart_lua_function_hpp

#include "../state.hpp"
#include "./caller.hpp"

#include "./type.hpp"

namespace fart::lua::types {

	class LuaFunction : public LuaType {

		friend class LuaType;
		friend class Strong<LuaFunction>;
		friend class Caller;

		public:

			class User {

				public:

					virtual ~User();

				private:

					static int _callback(lua_State* L);

					User(
						State& state,
						const ::function<Strong<Type>(const Array<Type>& arguments)> function);

					const ::function<Strong<Type>(const Array<Type>& arguments)> _function;

			};

			virtual ~LuaFunction();

			virtual LuaType::Kind kind() const override;

			Caller call();

		private:

			LuaFunction(
				State& state);

	};

}

#endif /* fart_lua_function_hpp */
