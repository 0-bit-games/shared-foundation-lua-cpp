//
// runtime-exception.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/06
// See license in LICENSE.
//

#ifndef fart_lua_runtime_exception_hpp
#define fart_lua_runtime_exception_hpp

#include "../fart/fart.hpp"

namespace fart::lua::exceptions {

	class RuntimeException : public fart::exceptions::Exception {

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

#endif /* fart_lua_runtime_exception_hpp */
