//
// compiler-exception.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/14
// See license in LICENSE.
//

#ifndef fart_lua_compiler_exception_hpp
#define fart_lua_compiler_exception_hpp

#include "../fart/fart.hpp"

namespace fart::lua::exceptions {

	class CompilerException : public fart::exceptions::Exception {

		public:

			CompilerException();

			CompilerException(
				const char* message);
			virtual ~CompilerException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

		private:

			const char* _message;

	};

}

#endif /* fart_lua_compiler_exception_hpp */
