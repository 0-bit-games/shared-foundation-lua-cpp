//
// compiler-exception.hpp
// foundation-lua
//
// Created by Kristian Trenskow on 2023/11/14
// See license in LICENSE.
//

#ifndef foundation_lua_compiler_exception_hpp
#define foundation_lua_compiler_exception_hpp

#include "../foundation/src/foundation.hpp"

namespace foundation::lua::exceptions {

	class CompilerException : public foundation::exceptions::Exception {

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

#endif /* foundation_lua_compiler_exception_hpp */
