//
// value.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#ifndef fart_lua_value_hpp
#define fart_lua_value_hpp

#include "./type.hpp"

namespace fart::lua::types {

	class LuaValue : public LuaType {

		friend class Strong<LuaValue>;
		friend class fart::lua::State;

		public:

			virtual ~LuaValue();

			virtual String kindDescription() const override;

			virtual Strong<LuaType> underlying() const override;

		private:

			LuaValue(
				State& state,
				const LuaType& underlying);

			Strong<LuaType> _underlying;

	};

}

#endif /* fart_lua_value_hpp */