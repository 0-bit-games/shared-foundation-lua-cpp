//
// string.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef foundation_lua_string_hpp
#define foundation_lua_string_hpp

#include "./type.hpp"

namespace foundation::lua::types {

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

#endif /* foundation_lua_string_hpp */
