//
// caller.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/10
// See license in LICENSE.
//

#ifndef foundation_lua_caller_hpp
#define foundation_lua_caller_hpp

#include "./type.hpp"

namespace foundation::lua::types {

	class LuaFunction;

	class Caller {

		friend class LuaFunction;

		public:

			Caller& argument(
				LuaBoolean& value);

			Caller& argument(
				LuaFunction& value);

			Caller& argument(
				LuaNumber& value);

			Caller& argument(
				double value);

			Caller& argument(
				int64_t value);

			Caller& argument(
				LuaString& value);

			Caller& argument(
				LuaTable& value);

			Caller& argument(
				const Type& value);

			Strong<Array<LuaType>> exec(
				double timeout = 0.0
			) const noexcept(false);

		private:

			Caller(
				LuaFunction& function);

			LuaFunction& _function;
			Array<LuaType> _arguments;

	};

}

#endif /* foundation_lua_caller_hpp */
