//
// boolean.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef foundation_lua_boolean_hpp
#define foundation_lua_boolean_hpp

#include "./type.hpp"

namespace foundation::lua::types {

	class LuaBoolean : public LuaType {

		friend class LuaType;
		friend class Strong<LuaBoolean>;

		public:

			virtual ~LuaBoolean();

			virtual LuaType::Kind kind() const override;

			bool value() const;

		private:

			LuaBoolean(
				State& state);

	};

}

#endif /* foundation_lua_boolean_hpp */
