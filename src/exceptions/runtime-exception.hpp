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
#include "../debug-information.hpp"

using namespace foundation::lua;

namespace foundation::lua::exceptions {

	class RuntimeException : public foundation::exceptions::Exception {

		public:

			RuntimeException(
				const String& message,
				Array<DebugInformation> stackTrace);

			virtual ~RuntimeException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

			const String& message() const;
			Array<DebugInformation> stackTrace() const;

		private:

			String _message;
			Array<DebugInformation> _stackTrace;

	};

}

#endif /* foundation_lua_runtime_exception_hpp */
