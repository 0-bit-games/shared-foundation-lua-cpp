//
// function.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef foundation_lua_function_hpp
#define foundation_lua_function_hpp

#include "../state.hpp"
#include "./caller.hpp"

#include "./type.hpp"

namespace foundation::lua::types {

	class LuaFunction : public LuaType {

		friend class LuaType;
		friend class Strong<LuaFunction>;
		friend class Caller;

		public:

			virtual ~LuaFunction();

			virtual LuaType::Kind kind() const override;

			Caller call();

		protected:

			LuaFunction(
				State& state);

	};

}

#endif /* foundation_lua_function_hpp */
