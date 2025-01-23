//
// not-supported-exception.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef foundation_lua_not_supported_exception_hpp
#define foundation_lua_not_supported_exception_hpp

#include "../foundation/src/foundation.hpp"

namespace foundation::lua::exceptions {

	class NotSupportedException : public foundation::exceptions::Exception {

		public:

			NotSupportedException();
			virtual ~NotSupportedException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

	};

}

#endif /* foundation_lua_not_supported_exception_hpp */
