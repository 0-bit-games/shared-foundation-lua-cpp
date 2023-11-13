//
// not-supported-exception.hpp
// fart-lua
//
// Created by Kristian Trenskow on 2023/11/09
// See license in LICENSE.
//

#ifndef fart_lua_not_supported_exception_hpp
#define fart_lua_not_supported_exception_hpp

#include <fart.hpp>

namespace fart::lua::exceptions {

	class NotSupportedException : public fart::exceptions::Exception {

		public:

			NotSupportedException();
			virtual ~NotSupportedException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

	};

}

#endif /* fart_lua_not_supported_exception_hpp */
