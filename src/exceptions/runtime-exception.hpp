//
// runtime-exception.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/06
// See license in LICENSE.
//

#ifndef foundation_lua_runtime_exception_hpp
#define foundation_lua_runtime_exception_hpp

#include "../foundation/src/foundation.hpp"

namespace foundation::lua::exceptions {

	class RuntimeException : public foundation::exceptions::Exception {

		public:

			RuntimeException(
				const String& message);
			virtual ~RuntimeException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

			const String& message() const;

		private:

			String _message;

	};

}

#endif /* foundation_lua_runtime_exception_hpp */
