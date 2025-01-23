//
// value.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/21
// See license in LICENSE.
//

#ifndef foundation_lua_value_hpp
#define foundation_lua_value_hpp

#include "./type.hpp"

namespace foundation::lua::types {

	class LuaValue : public LuaType {

		friend class Strong<LuaValue>;
		friend class foundation::lua::State;

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

#endif /* foundation_lua_value_hpp */