//
// compiler-exception.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/14
// See license in LICENSE.
//

#ifndef fart_lua_compiler_exception_hpp
#define fart_lua_compiler_exception_hpp

#include <fart.hpp>

namespace fart::lua::exceptions {

	class CompilerException : public fart::exceptions::Exception {

		public:

			CompilerException();
			virtual ~CompilerException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

	};

}

#endif /* fart_lua_compiler_exception_hpp */
