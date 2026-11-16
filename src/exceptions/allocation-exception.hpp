//
// allocation-exception.hpp
// 0-bit-games-shared-foundation-lua-cpp
//
// Created by Kristian Trenskow on 2025/11/16
// See license in LICENSE.
//

#ifndef foundation_lua_allocation_exception_hpp
#define foundation_lua_allocation_exception_hpp

#include "../foundation/src/foundation.hpp"

namespace foundation::lua::exceptions {

	class AllocationException : public foundation::exceptions::Exception {

		public:

			AllocationException();
			virtual ~AllocationException();

			virtual const char* description() const override;
			virtual Exception* clone() const override;

	};

}

#endif /* foundation_lua_allocation_exception_hpp */
