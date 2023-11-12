//
// boolean.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef fart_lua_boolean_hpp
#define fart_lua_boolean_hpp

#include "./type.hpp"

namespace fart::lua::types {

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

#endif /* fart_lua_boolean_hpp */
