//
// string.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef fart_lua_string_hpp
#define fart_lua_string_hpp

#include "./type.hpp"

namespace fart::lua::types {

	class LuaString : public LuaType {

		friend class LuaType;
		friend class Strong<LuaString>;

		public:

			virtual ~LuaString();

			virtual LuaType::Kind kind() const override;

			Strong<String> value() const;

		private:

			LuaString(
				State& state);

	};

}

#endif /* fart_lua_string_hpp */
