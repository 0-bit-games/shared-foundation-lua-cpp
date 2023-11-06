//
// lua-exception.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/06
// See license in LICENSE.
//

#ifndef lau_exception_hpp
#define lau_exception_hpp

#include "./fart/fart.hpp"

namespace fart::lua::exceptions {

	class LuaException : public fart::exceptions::Exception {

		public:
			LuaException(
				const String& message);
			virtual ~LuaException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

			const String& message() const;

		private:

			String _message;

	};

}

#endif /* lau_exception_hpp */
