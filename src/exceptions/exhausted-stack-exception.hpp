//
// exhausted-exception.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2025/02/06
// See license in LICENSE.
//

#ifndef foundation_lua_exhausted_exception_hpp
#define foundation_lua_exhausted_exception_hpp

#include "../foundation/src/foundation.hpp"

namespace foundation::lua::exceptions {

	class ExhaustedStackException : public foundation::exceptions::Exception {

		public:

			ExhaustedStackException();
			virtual ~ExhaustedStackException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

	};

}

#endif /* foundation_lua_exhausted_exception_hpp */
