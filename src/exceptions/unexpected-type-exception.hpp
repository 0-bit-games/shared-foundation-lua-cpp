//
// unexpected-type-exception.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef foundation_lua_unexpected_type_exception_hpp
#define foundation_lua_unexpected_type_exception_hpp

#include "../foundation/src/foundation.hpp"

#include "../types/types.hpp"

using namespace foundation::lua::types;

namespace foundation::lua::exceptions {

	class UnexpectedTypeException : public foundation::exceptions::Exception {

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

#endif /* foundation_lua_unexpected_type_exception_hpp */
