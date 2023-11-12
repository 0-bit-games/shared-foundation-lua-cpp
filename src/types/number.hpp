//
// number.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef fart_lua_number_hpp
#define fart_lua_number_hpp

#include "./type.hpp"

namespace fart::lua::types {

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

#endif /* fart_lua_number_hpp */
