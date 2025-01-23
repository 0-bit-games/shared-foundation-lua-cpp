//
// number.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef foundation_lua_number_hpp
#define foundation_lua_number_hpp

#include "./type.hpp"

namespace foundation::lua::types {

	class LuaNumber : public LuaType {

		friend class LuaType;
		friend class Strong<LuaNumber>;

		public:

			virtual ~LuaNumber();

			virtual LuaType::Kind kind() const override;

			bool isInteger() const;

			double value() const;

		private:

			LuaNumber(
				State& state);

	};

}

#endif /* foundation_lua_number_hpp */
