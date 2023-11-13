//
// unexpected-type-exception.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef fart_lua_unexpected_type_exception_hpp
#define fart_lua_unexpected_type_exception_hpp

#include <fart.hpp>

#include "../types/types.hpp"

using namespace fart::lua::types;

namespace fart::lua::exceptions {

	class UnexpectedTypeException : public fart::exceptions::Exception {

		public:

			UnexpectedTypeException(
				LuaType::Kind expectedKind,
				LuaType::Kind encounteredKind);
			virtual ~UnexpectedTypeException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

			LuaType::Kind expectedKind() const;
			LuaType::Kind encounteredKind() const;

		private:

			LuaType::Kind _expectedKind;
			LuaType::Kind _encounteredKind;

	};

}

#endif /* fart_lua_unexpected_type_exception_hpp */
