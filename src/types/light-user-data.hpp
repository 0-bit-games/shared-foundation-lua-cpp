//
// light-user-data.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef fart_lua_light_user_data_hpp
#define fart_lua_light_user_data_hpp

#include "../state.hpp"

#include "./type.hpp"

namespace fart::lua::types {

	class LuaLightUserData : public LuaType {

		friend class LuaType;
		friend class Strong<LuaLightUserData>;

		public:

			virtual ~LuaLightUserData();

			virtual LuaType::Kind kind() const override;

			void* value() const;

		private:

			LuaLightUserData(
				State& state);

	};

}

#endif /* fart_lua_light_user_data_hpp */